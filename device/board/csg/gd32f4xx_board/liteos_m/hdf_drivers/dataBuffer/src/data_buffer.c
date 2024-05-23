/*
 * Copyright (c) 2022 Hihope Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdio.h>
#include "data_buffer.h"

/*******************************************************************
 Function Name: buffer_deinit
 Functional description: Buffer deinitialization
 Parameters input: struct rbuffer *buf: buffer handle
 Parameters output: 
 Return value: 
 Note:
*******************************************************************/
void buffer_deinit(struct buffer_t *buf)
{
    uint32_t i;

    if(buf == NULL && buf->pbuf == NULL) {
        printf("buf or buf->pbuf is NULL, Deinit failed!\n");
        return;
    }

    OsalMemFree(buf->pbuf);

    buf->put = 0;
    buf->get = 0;
    buf->size = 0;
}

/*******************************************************************
 Function Name: buffer_init
 Functional description: Buffer initialization
 Parameters input: struct rbuffer *buf: buffer handle
 Parameters output: 
 Return value: 
 Note:
*******************************************************************/
void buffer_init(struct buffer_t *buf, uint32_t size)
{
    uint32_t i;

    if((buf == NULL) || (size <= 0)) {
        printf("buf is NULL or size is 0, Init failed!\n");
        return;
    }

    buf->pbuf = (uint8_t *)OsalMemAlloc(size);
    if(buf->pbuf == NULL) {
        printf("no MemAlloc, Init failed!\n");
        return;
    }

    memset_s(buf->pbuf, size, 0, size);

    buf->put = 0;
    buf->get = 0;
    buf->size = size;
}

/*******************************************************************
 Function Name: buffer_putc
 Functional description: Pushes a byte into the buffer queue
 Parameters input: struct buffer_t *buf: buffer handle
 char ch: push byte
 Parameters output: 
 Return value: 
 Note:
*******************************************************************/
void buffer_putc(struct buffer_t *buf, char ch)
{
    if(buf == NULL) {
        printf("%s: buf is NULL\n", __func__);
        return;
    }

    buf->pbuf[buf->put++] = ch;
    if (buf->put >= buf->size) buf->put = 0;

    /* if the next position is read index, discard this 'read char' */
    if (buf->put == buf->get)
    {
        buf->get += 1;
        if (buf->get >= buf->size) buf->get = 0;
    }
}

/*******************************************************************
 Function Name: get_buf_size
 Functional description: get used buffer size
 Parameters input: struct buffer_t *buf: buffer handle
 Parameters output:
 Return value: used size
 Note:
*******************************************************************/
uint32_t get_buff_used_size(struct buffer_t *buf)
{
    if(buf == NULL) {
        printf("%s: buf is NULL\n", __func__);
        return -1;
    }

    int32_t size;

    size = buf->put - buf->get;
    if (size < 0) {
        size = buf->size + size;
    }

    return size;
}

/*******************************************************************
 Function Name: get_nchar_cpy
 Functional description: Gets the specified number of bytes from the buffer in copy mode
 Parameters input: struct buffer_t *buf: buffer handle(source)
                   uint8_t *out_buf: buffer address(dest)
                   uint8_t size: get data size
 Parameters output: 
 Return value: The byte size has been obtained
 Note:  In copy mode, the data can still be obtained the next time
 *******************************************************************/
static uint32_t get_nchar_cpy(struct buffer_t *buf, uint8_t *out_buf, uint32_t size)
{
    uint32_t size_bak;
    uint32_t index;

    if(buf == NULL || out_buf == NULL) {
        printf("%s: buf is NULL\n", __func__);
        return -1;
    }

    if(size < 0) {
        printf("%s: size < 0\n", __func__);
        return -1;
    }

    index = buf->get;
    size_bak = size;

    while (size)
    {
        *out_buf++ = buf->pbuf[index++];
        if (index >= buf->size) index = 0;
        size--;
    }

    return (size_bak - size);
}

/*******************************************************************
 Function Name: get_nchar_cut
 Functional description: Gets the specified number of bytes from the buffer in cut mode
 Parameters input: struct buffer_t *buf: buffer handle
                   uint8_t *out_buf: data address(dest)
                   uint8_t size: data size
 Parameters output: 
 Return value: The byte size has been obtained
 Note:  In cut mode, the data can't still be obtained the next time
 *******************************************************************/
static uint32_t get_nchar_cut(struct buffer_t *buf, uint8_t *out_buf, uint32_t size)
{
    uint32_t size_bak;

    if(buf == NULL || out_buf == NULL) {
        printf("%s: buf is NULL\n", __func__);
        return -1;
    }

    if(size < 0) {
        printf("%s: size < 0\n", __func__);
        return -1;
    }

    size_bak = size;

    while (size)
    {
        *out_buf++ = buf->pbuf[buf->get++];
        if (buf->get >= buf->size) buf->get = 0;
        size--;
    }

    return (size_bak - size);
}

/*******************************************************************
 Function Name: buff_read
 Functional description: read data by buffer
 Parameters input: struct buffer_t *buf: buffer handle
                    type_e type: read attubrite
                    uint8_t *out: data address(dest)
                    uint8_t size: get data size
 Parameters output: 
 Return value: actual read data size
 Note: Gets the data size if there is not enough existing data in the buffer, return 0
 *******************************************************************/
uint32_t buff_read(struct buffer_t *buf, uint8_t *out, uint32_t size, type_e type)
{
    uint32_t read_nbytes, buf_size;

    if(buf == NULL || out == NULL) {
        printf("%s: buf or out is NULL\n", __func__);
        return -1;
    }

    if (size == 0)
    {
        return 0;
    }

    buf_size = get_buff_used_size(buf);
    if (buf_size > size)
    {
        buf_size = size;
    }

    switch(type)
    {
        case CPY:
            read_nbytes = get_nchar_cpy(buf, out, buf_size);
            break;

        case CUT:
            read_nbytes = get_nchar_cut(buf, out, buf_size);
            break;

        default:
            /* do nothing */
            break;
    }

    return read_nbytes;
}

/*******************************************************************
 Function Name: buff_write
 Functional description: write data by buffer
 Parameters input: struct buffer_t *buf  buffer handle
                    const void *in  data address
                    uint8_t size: write data size
 Parameters output: 
 Return value: actual write data size
 Note: 
*******************************************************************/
uint32_t buff_write(struct buffer_t *buf, const uint8_t *in, uint32_t size)
{
    uint32_t size_bak;

    if(buf == NULL || in == NULL) {
        printf("%s: buf or in is NULL\n", __func__);
        return -1;
    }

    if (size == 0)
    {
        return -1;
    }


    size_bak = size;
    while(size)
    {
        buffer_putc(buf, *in);
        size--;
        in++;
    }
    return (size_bak - size);
}


/**
 * zero copy: only update the put(even get if overlap) index
 * without touching the buffer, which is filed by DMA
 * @param buf the buffer object
 * @param new_put the new put pos from DMA NDTR
 * note: if put==get, means empty, if put is less than get one time, mean full
 */
void buffer_put_zero(struct buffer_t *buf, uint32_t new_put)
{
    int32_t put_forward;    // distance between new_put and put
    int32_t dis_put_get;    // distance between put and get
    int32_t dis_new_put_get;// distance between new_put and get

    put_forward = new_put - buf->put;
    if (put_forward < 0) {
        put_forward =  buf->size + put_forward;
    }
    dis_put_get = buf->put - buf->get;
    if (dis_put_get < 0) {
        dis_put_get =  buf->size + dis_put_get;
    }

    dis_new_put_get = dis_put_get + put_forward;
    if (dis_new_put_get >= buf->size) {
        int get_forward = (dis_new_put_get + 1) % buf->size;
        buf->get = (buf->get + get_forward) % buf->size;
    }

    buf->put = new_put;
}
