#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ws.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


int current_ws_fd = -1;
int mem_fd = 0;
uint32_t* mem_map = NULL;
uint32_t mapped_size = 0;
uint8_t* mem_map_offset = NULL;

/**
 * @brief This function is called whenever a new connection is opened.
 * @param fd The new client file descriptor.
 */
void onopen(int fd)
{
    char *cli;
    cli = ws_getaddress(fd);
    printf("Connection opened, client: %d | addr: %s\n", fd, cli);
    free(cli);
    if (current_ws_fd != -1) {
        printf("There is already opened connection (client: %d), closing it...\n", current_ws_fd);
    }
    current_ws_fd = fd;
}

/**
 * @brief This function is called whenever a connection is closed.
 * @param fd The client file descriptor.
 */
void onclose(int fd)
{
    char *cli;
    cli = ws_getaddress(fd);
    printf("Connection closed, client: %d | addr: %s\n", fd, cli);
    free(cli);
    current_ws_fd = -1;
}

uint8_t configure_mmap(uint32_t target, uint32_t width)
{
    if (mem_map != NULL) {
        munmap(mem_map, mapped_size);
    }
    uint32_t page_size, offset_in_page;

    mapped_size = page_size = getpagesize();
    offset_in_page = (unsigned)target & (page_size - 1);
    if (offset_in_page + width > page_size) {
        mapped_size *= 2;
    }
    mem_map = mmap(NULL,
            mapped_size,
            (PROT_READ | PROT_WRITE),
            MAP_SHARED,
            mem_fd,
            target & ~(uint32_t)(page_size - 1));
    if (mem_map == MAP_FAILED) {
        return 2;
    }

    mem_map_offset = (uint8_t*)mem_map + offset_in_page;
    return 0;
}

void send_message(uint8_t type, uint8_t message_id, uint8_t error_code, uint8_t* data, uint32_t length)
{
    uint8_t* buf = malloc(7 + length);
    buf[0] = type;
    buf[1] = message_id;
    buf[2] = error_code;
    *((uint32_t*)(buf + 3)) = length;
    if (length) {
        memcpy(buf + 7, data, length);
    }
    ws_sendframe_bin(current_ws_fd, (const char*)buf, 7 + length, false);
    free(buf);
}

/**
 * @brief Message events goes here.
 * @param fd   Client file descriptor.
 * @param msg  Message content.
 * @param size Message size.
 * @param type Message type.
 */
void onmessage(int fd, const unsigned char *msg, size_t size, int type)
{
    uint8_t msg_type = msg[0];
    uint8_t msg_id = msg[1];
    uint32_t msg_length = *((uint32_t*)(msg + 2));
    const uint8_t* msg_data = msg + 6;
    
    printf("Got message %X!\n", msg_type);
    if (msg_type == 0x00) { // Get Page Size
        uint32_t page_size = getpagesize();
        send_message(msg_type, msg_id, 0, (uint8_t*)&page_size, 4);
    } else if (msg_type == 0x01) {
        uint32_t address = *((uint32_t*)(msg_data));
        uint32_t length = *((uint32_t*)(msg_data + 4));
        int8_t err = configure_mmap(address, length);
        send_message(msg_type, msg_id, err, NULL, 0);
    } else if (msg_type == 0x02) {
        uint32_t offset = *((uint32_t*)(msg_data));
        uint32_t length = *((uint32_t*)(msg_data + 4));
        uint8_t* buf = malloc(length);
        memcpy(buf, mem_map_offset + offset, length);
        send_message(msg_type, msg_id, 0, buf, length);
        free(buf);
    }

    /*if (msg[0] == 0x02) {
        uint32_t address = *(uint32_t*)(msg + 1);
        uint32_t length = *(uint32_t*)(msg + 5);
        uint8_t* read_result = get_mem(address, length);
        ws_sendframe_bin(fd, (const char*)read_result, length, false);
        free(read_result);
    }*/
    /*char *cli;
    cli = ws_getaddress(fd);
    printf("I receive a message: %s (%zu), from: %s/%d\n", msg,
        size, cli, fd);

    sleep(2);
    ws_sendframe_txt(fd, "hello", false);
    sleep(2);
    ws_sendframe_txt(fd, "world", false);

    free(cli);*/
}

int main()
{
    mem_fd = open("/dev/mem", (O_RDWR | O_SYNC), 0666);
    /* Register events. */
    struct ws_events evs;
    evs.onopen    = &onopen;
    evs.onclose   = &onclose;
    evs.onmessage = &onmessage;

    ws_socket(&evs, 8080);

    return (0);
}