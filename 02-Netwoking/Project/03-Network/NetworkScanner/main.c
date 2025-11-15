#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#define MAX_PORT 65535
#define DEFAULT_THREADS 50
#define TIMEOUT_SEC 2
#define TIMEOUT_USEC 0

typedef struct {
    char *target_host;
    unsigned int start_port;
    unsigned int end_port;
    int *open_ports;
    int open_count;
    pthread_mutex_t lock;
} scanner_t;

// Function to resolve hostname to IP address
char* resolve_hostname(const char *hostname) {
    struct hostent *host;
    struct in_addr **addr_list;
    
    if ((host = gethostbyname(hostname)) == NULL) {
        return NULL;
    }
    
    addr_list = (struct in_addr **)host->h_addr_list;
    return inet_ntoa(*addr_list[0]);
}

// Function to check if a port is open
int check_port(const char *ip, unsigned int port) {
    int sock;
    struct sockaddr_in target;
    struct timeval timeout;
    
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }
    
    // Set socket options for timeout
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    
    // Configure target address
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    target.sin_addr.s_addr = inet_addr(ip);
    
    // Attempt connection
    if (connect(sock, (struct sockaddr *)&target, sizeof(target)) == 0) {
        close(sock);
        return 1; // Port is open
    }
    
    close(sock);
    return 0; // Port is closed or filtered
}

// Thread function for port scanning
void* scan_ports(void *arg) {
    scanner_t *scanner = (scanner_t *)arg;
    char *ip = resolve_hostname(scanner->target_host);
    
    if (ip == NULL) {
        fprintf(stderr, "Error: Could not resolve hostname '%s'\n", scanner->target_host);
        return NULL;
    }
    
    printf("Scanning %s (%s) from port %u to %u\n", 
           scanner->target_host, ip, scanner->start_port, scanner->end_port);
    
    for (unsigned int port = scanner->start_port; port <= scanner->end_port; port++) {
        int result = check_port(ip, port);
        
        if (result == 1) {
            pthread_mutex_lock(&scanner->lock);
            scanner->open_ports[scanner->open_count++] = port;
            printf("Port %u is OPEN\n", port);
            pthread_mutex_unlock(&scanner->lock);
        } else if (result == -1) {
            fprintf(stderr, "Error scanning port %u: %s\n", port, strerror(errno));
        }
        
        // Small delay to be more network-friendly
        usleep(1000);
    }
    
    return NULL;
}

// Multi-threaded port scanner
void threaded_port_scan(const char *target_host, unsigned int start_port, 
                       unsigned int end_port, int num_threads) {
    pthread_t *threads;
    scanner_t *scanners;
    int ports_per_thread;
    int remaining_ports;
    
    if (start_port > end_port || start_port == 0 || end_port > MAX_PORT) {
        fprintf(stderr, "Error: Invalid port range\n");
        return;
    }
    
    if (num_threads <= 0) {
        num_threads = DEFAULT_THREADS;
    }
    
    int total_ports = end_port - start_port + 1;
    ports_per_thread = total_ports / num_threads;
    remaining_ports = total_ports % num_threads;
    
    // Allocate memory
    threads = malloc(num_threads * sizeof(pthread_t));
    scanners = malloc(num_threads * sizeof(scanner_t));
    int *open_ports = malloc(total_ports * sizeof(int));
    
    if (!threads || !scanners || !open_ports) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(threads);
        free(scanners);
        free(open_ports);
        return;
    }
    
    // Initialize scanner structures
    int current_port = start_port;
    for (int i = 0; i < num_threads; i++) {
        scanners[i].target_host = (char*)target_host;
        scanners[i].start_port = current_port;
        
        int thread_ports = ports_per_thread;
        if (i < remaining_ports) {
            thread_ports++;
        }
        
        scanners[i].end_port = current_port + thread_ports - 1;
        scanners[i].open_ports = open_ports;
        scanners[i].open_count = 0;
        pthread_mutex_init(&scanners[i].lock, NULL);
        
        current_port += thread_ports;
    }
    
    // Create threads
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, scan_ports, &scanners[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
        }
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Cleanup
    int total_open = 0;
    for (int i = 0; i < num_threads; i++) {
        total_open += scanners[i].open_count;
        pthread_mutex_destroy(&scanners[i].lock);
    }
    
    // Print summary
    printf("\n=== SCAN RESULTS ===\n");
    printf("Target: %s\n", target_host);
    printf("Port range: %u-%u\n", start_port, end_port);
    printf("Open ports found: %d\n", total_open);
    
    if (total_open > 0) {
        printf("Open ports: ");
        for (int i = 0; i < total_open; i++) {
            printf("%d", open_ports[i]);
            if (i < total_open - 1) printf(", ");
        }
        printf("\n");
    }
    
    free(threads);
    free(scanners);
    free(open_ports);
}

// Simple sequential port scanner (single-threaded)
void simple_port_scan(const char *target_host, unsigned int start_port, unsigned int end_port) {
    char *ip = resolve_hostname(target_host);
    
    if (ip == NULL) {
        fprintf(stderr, "Error: Could not resolve hostname '%s'\n", target_host);
        return;
    }
    
    printf("Scanning %s (%s) from port %u to %u\n", target_host, ip, start_port, end_port);
    
    int open_ports[MAX_PORT];
    int open_count = 0;
    
    for (unsigned int port = start_port; port <= end_port; port++) {
        printf("\rScanning port %u...", port);
        fflush(stdout);
        
        int result = check_port(ip, port);
        
        if (result == 1) {
            open_ports[open_count++] = port;
            printf("\rPort %u is OPEN\n", port);
        } else if (result == -1) {
            fprintf(stderr, "\rError scanning port %u: %s\n", port, strerror(errno));
        }
        
        usleep(10000); // 10ms delay
    }
    
    printf("\n\n=== SCAN RESULTS ===\n");
    printf("Target: %s\n", target_host);
    printf("Port range: %u-%u\n", start_port, end_port);
    printf("Open ports found: %d\n", open_count);
    
    if (open_count > 0) {
        printf("Open ports: ");
        for (int i = 0; i < open_count; i++) {
            printf("%d", open_ports[i]);
            if (i < open_count - 1) printf(", ");
        }
        printf("\n");
    }
}

void print_usage(const char *program_name) {
    printf("Usage: %s <target> [start_port] [end_port] [threads]\n", program_name);
    printf("  target     : Hostname or IP address to scan\n");
    printf("  start_port : Starting port (default: 1)\n");
    printf("  end_port   : Ending port (default: 1024)\n");
    printf("  threads    : Number of threads (default: 50, use 1 for single-threaded)\n");
    printf("\nExamples:\n");
    printf("  %s example.com\n", program_name);
    printf("  %s 192.168.1.1 1 1000 10\n", program_name);
    printf("  %s localhost 80 100 1\n", program_name);
}

int main(int argc, char *argv[]) {
    char *target_host;
    unsigned int start_port = 1;
    unsigned int end_port = 1024;
    int num_threads = DEFAULT_THREADS;
    
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    target_host = argv[1];
    
    // Parse optional arguments
    if (argc >= 3) start_port = atoi(argv[2]);
    if (argc >= 4) end_port = atoi(argv[3]);
    if (argc >= 5) num_threads = atoi(argv[4]);
    
    // Validate input
    if (start_port == 0 || end_port == 0 || start_port > end_port || end_port > MAX_PORT) {
        fprintf(stderr, "Error: Invalid port range\n");
        return 1;
    }
    
    printf("Starting port scan...\n");
    printf("Target: %s\n", target_host);
    printf("Port range: %u-%u\n", start_port, end_port);
    printf("Threads: %d\n", num_threads);
    printf("========================\n");
    
    if (num_threads == 1) {
        // Use single-threaded scanner
        simple_port_scan(target_host, start_port, end_port);
    } else {
        // Use multi-threaded scanner
        threaded_port_scan(target_host, start_port, end_port, num_threads);
    }
    
    return 0;
}
