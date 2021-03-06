#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdbool.h>
#include <cmp/cmp.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


/** Version of the protocol command set. */
#define COMMAND_SET_VERSION 3

/** Total number of supported commands */
#define COMMAND_COUNT 10


/**
 * This struct allows for the definition of bootloader commands
 * and association of the corresponding handlers/callbacks
 * to be executed upon invocation of the command via CAN.
 */
typedef struct {
    /** Command ID */
    uint8_t index;
    /** The callback function for this entry.
     *
     * @param [in] argc Argument count
     * @param [in] arg_context MessagePack context containing the parameters.
     * @param [in] out_context MessagePack context in which the output data should be written.
     * @param [in] config A bootloader config instance.
     */
    void (*callback)(int, cmp_ctx_t *, cmp_ctx_t *, bootloader_config_t *config);
} command_t;


/** Find a command by command index
 * @param [in] index The command index to search
 * @return Command with the matching index
 * @retval 0 No command with a matching index was found
 */
command_t* get_command_by_index(uint8_t index);


/** Parses a datagram data field and executes the correct function.
 * @param [in] data The raw data to parse.
 * @param [in] data_len Length of data.
 * @param [in] commands A list of all possible commands.
 * @param [in] command_len Length of the commands array.
 * @param [in] config A pointer to a bootloader configuration structure that will be passed to the commands.
 * @param [out] out_buf A buffer where the command can place its results, which will be sent back to the client.
 * @param [in] out_len Length of out_buf.
 * @returns The amount of bytes written to output_buffer on successful command completion.
 * @returns A negative error code if the command execution encountered an error.
 */
int execute_datagram_commands(char *data, size_t data_len, const command_t *commands, int command_len, char *out_buf, size_t out_len, bootloader_config_t *config);


/** Command used to erase a flash page.
 *
 * @note Should not be called directly but be a part of the commands given to protocol_execute_command.
 */
void command_erase_flash_page(int argc, cmp_ctx_t *args, cmp_ctx_t *out, bootloader_config_t *config);


/** Command used to write to a flash page.
 *
 * @note Should not be called directly but be a part of the commands given to protocol_execute_command.
 */
void command_write_flash(int argc, cmp_ctx_t *args, cmp_ctx_t *out, bootloader_config_t *config);


/** Command used to read from flash.
 *
 *  @note Should not be called directly but be a part of the commands given to protocol_execute_command.
 */
void command_read_flash(int argc, cmp_ctx_t *args, cmp_ctx_t *out, bootloader_config_t *config);


/** Command used to compute the CRC of a flash page. */
void command_crc_region(int argc, cmp_ctx_t *args, cmp_ctx_t *out, bootloader_config_t *config);


/** Command used to jump to the application code.
 *
 * @note Should not be called directly but be a part of the commands given to protocol_execute_command.
 */
void command_jump_to_application(int argc, cmp_ctx_t *args, cmp_ctx_t *out, bootloader_config_t *config);


/** Command used to update entries of confit in RAM. */
void command_config_update(int argc, cmp_ctx_t *args, cmp_ctx_t *out, bootloader_config_t *config);


/** Command used to write config to flash. */
void command_config_write_to_flash(int argc, cmp_ctx_t *args, cmp_ctx_t *out, bootloader_config_t *config);


/** Reads the current config and sends as MessagePack encoded map. */
void command_config_read(int argc, cmp_ctx_t *args, cmp_ctx_t *out, bootloader_config_t *config);


/** Ping command. Simply replies with true. */
void command_ping(int argc, cmp_ctx_t *args, cmp_ctx_t *out, bootloader_config_t *config);


/** Allows to set the current status code */
void set_status(uint8_t code);


/** Command requesting last status code */
void command_get_status(int argc, cmp_ctx_t *args, cmp_ctx_t *out, bootloader_config_t *config);


#ifdef __cplusplus
}
#endif

#endif
