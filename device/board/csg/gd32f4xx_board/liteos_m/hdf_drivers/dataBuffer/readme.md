#dataBuffer

针对 循环buffer对象的读写操作：
```
struct buffer_t
{
    uint32_t put;   // pbuf的写索引
    uint32_t get;   // pbuf的读索引
    uint32_t size;  // pbuf的大小
    uint8_t *pbuf;  // pbuf指针
};
```

## API
具体API详见头文件

## Note
1. 当写速度大于读时，老的数据会被覆盖，因此调用者需要保证读写平衡，避免覆盖场景
* 当get==put时，为空
* 当get=(put+1) % size 时，为满

2. 针对DMA写操作的场景，增加buffer_put_zero，即仅更新put/get索引，此场景的UT用例如下:
```
    struct buffer_t bufObj;
    printf("test suite for buffer_put_zero: stimulate all the possible put/get/newput orders\n description:\n");
    printf("     get - curent get \n");
    printf("     put - current put \n");
    printf("     newput - new put which will replace the current put \n");
    printf("     cause the put is exact the newput, no need to be verified, so \n");
    printf("     focus the the obj->get - the new get avoid overlap \n");

    buffer_init(&bufObj, 10);
    buffer_put_zero(&bufObj, 5);
    printf("get0-put0-newput5 >>> obj->get=%d, should be 0\n", bufObj.get);

    bufObj.get = 4;
    buffer_put_zero(&bufObj, 8); // get-put-newput
    printf("get4-put5-newput8 >>> obj->get=%d, should be 4\n", bufObj.get);

    buffer_put_zero(&bufObj, 2); // newput-get-put
    printf("newput2-get4-put8- >>> obj->get=%d, should be 4\n", bufObj.get);

    bufObj.put = 6;
    buffer_put_zero(&bufObj, 5); // get - newput - put: overlap
    printf("get4-newput5-put6: >>> obj->get=%d, should be 6\n", bufObj.get);

    bufObj.get = 9;
    buffer_put_zero(&bufObj, 8); // put - newput -get
    printf("put5-newput8-get6 >>> obj->get=%d, should be 9\n", bufObj.get);

    buffer_put_zero(&bufObj, 7); // newput - put  -get: overlap 0
    printf("newput7-put8-get9 >>> obj->get=%d, should be 8\n", bufObj.get);

    bufObj.get = 7;
    bufObj.put = 6;
    buffer_put_zero(&bufObj, 9); // put - get - newput: overlap 1
    printf("put6-get7-newput9 >>> obj->get=%d, should be 0\n", bufObj.get);

    bufObj.get = 9;
    bufObj.put = 8;

    buffer_put_zero(&bufObj, 2); // put - get - newput: overlap 2
    printf("put8-get9-newput2 >>> obj->get=%d, should be 3\n", bufObj.get);
    buffer_deinit(&bufObj);
```
