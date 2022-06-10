#ifndef TRANSPORT_SERVER_H_
#define TRANSPORT_SERVER_H_

/** Message header request structure */
struct cmd_request_hdr {
    char cmd_name [100];
    uint32_t payload_size;
} __attribute__((packed));
/** Message Payload structure definitio*/
struct payload{
  char * buff;
  int offset;
} __attribute__((packed));
/* Message response structure definition */
struct test_msg_rep {
    char * resp_buff;
    //char  resp_buff[100];
} __attribute__((packed));

/** Server thread data */
struct server_data {
    pthread_t tid;
    zsock_t *server;
};
/**
 * Creates a generic command
 *
 * A command encapsulates the information required to
 * execute a command function.
 *
 * @param data    Private data to be passed to the command execute function
 * @param execute Pointer to the command execute function
 *
 * @return Pointer to a command structure.
 */
void* msg_server_fn(void *arg);


int serverExec(void);

#endif // TRANSPORT_SERVER_H_
