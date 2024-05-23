#ifndef __DATA_BUFFER_H__
#define __DATA_BUFFER_H__

#include <stdint.h>

typedef enum _read_type_e_
{
    CPY,
    CUT
}type_e;

/* buffer structure */
struct buffer_t
{
    uint32_t put;
    uint32_t get;
    uint32_t size;
    uint8_t *pbuf;
};

void buffer_init(struct buffer_t *buf, uint32_t size);
void buffer_deinit(struct buffer_t *buf);
uint32_t buff_read(struct buffer_t *buf, uint8_t *out, uint32_t size, type_e type);
uint32_t buff_write(struct buffer_t *buf, const uint8_t *in, uint32_t size);
uint32_t get_buff_used_size(struct buffer_t *buf);
void buffer_putc(struct buffer_t *buf, char ch);
void buffer_put_zero(struct buffer_t *buf, uint32_t new_put);

#endif  /* __DATA_BUFFER_H__ */

