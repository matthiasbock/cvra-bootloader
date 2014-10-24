#include "command.h"
#include "flash_writer.h"

// XXX Change page size
static char page_buffer[1024];

void (*application_main)(void);

void command_write_flash(int argc, cmp_ctx_t *args, cmp_ctx_t *out)
{
    void *adress;

    int8_t type;
    int size;

    bool success;

    cmp_read_uint(args, (void *)&adress);

    success = cmp_read_ext(args, &type, &size, page_buffer);

    if (!success) {
        return;
    }

    flash_writer_unlock();

    flash_writer_page_erase(adress);

    flash_writer_page_write(adress, page_buffer, size);

    flash_writer_lock();
}

void command_jump_to_application(int argc, cmp_ctx_t *args, cmp_ctx_t *out)
{
    application_main();
}

int protocol_execute_command(char *data, command_t *commands, int command_len, char *output_data)
{
    serializer_t serializer;
    cmp_ctx_t command_reader;
    int commmand_index, i;
    int argc;
    bool read_success;

    serializer_t out_serializer;
    cmp_ctx_t out_writer;

    // XXX size 0 should be replaced with correct size
    serializer_init(&serializer, data, 0);
    serializer_cmp_ctx_factory(&command_reader, &serializer);

    serializer_init(&out_serializer, output_data, 0);
    serializer_cmp_ctx_factory(&out_writer, &out_serializer);

    read_success = cmp_read_int(&command_reader, &commmand_index);

    if (!read_success) {
        return -ERR_INVALID_COMMAND;
    }

    read_success = cmp_read_array(&command_reader, &argc);

    /* If we cannot read array size, assume it is because we don't have
     * arguments. */
    if (!read_success) {
        argc = 0;
    }

    for (i = 0; i < command_len; ++i) {
        if (commands[i].index == commmand_index) {
            commands[i].callback(argc, &command_reader, &out_writer);
            return 0;
        }
    }

    return -ERR_COMMAND_NOT_FOUND;
}
