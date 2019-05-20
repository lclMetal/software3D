typedef struct ScreenStruct
{
    short width;
    short height;
    unsigned char* buffer;
}Screen;

Screen *newScreen(short scrWidth, short scrHeight)
{
    Screen *ptr = NULL;

    ptr = malloc(sizeof *ptr);

    if (!ptr) return NULL;

    ptr->width = scrWidth;
    ptr->height = scrHeight;
    ptr->buffer = malloc(sizeof *(ptr->buffer) * ptr->width * ptr->height * 3);

    if (!ptr->buffer)
    {
        free(ptr);
        return NULL;
    }

    return ptr;
}

void destroyScreen(Screen *ptr)
{
    if (!ptr) return;

    free(ptr->buffer);
    free(ptr);
}

void eraseScreen(Screen *ptr, unsigned char r, unsigned char g, unsigned char b)
{
    int i, j, k;

    if (!ptr) return;

    for (i = 0; i < ptr->height; i ++)
    {
        for (j = 0; j < ptr->width; j ++)
        {
            ptr->buffer[i * ptr->width * 3 + j * 3] = r;
            ptr->buffer[i * ptr->width * 3 + j * 3 + 1] = g;
            ptr->buffer[i * ptr->width * 3 + j * 3 + 2] = b;
        }
    }
}

void renderScreen(Screen *ptr)
{
    int i, j, k;

    if (!ptr) return;

    for (i = 0; i < ptr->height; i ++)
    {
        for (j = 0; j < ptr->width; j ++)
        {
            setpen(ptr->buffer[i * ptr->width * 3 + j * 3],
            ptr->buffer[i * ptr->width * 3 + j * 3 + 1],
            ptr->buffer[i * ptr->width * 3 + j * 3 + 2], 0, 1);
            putpixel(j, i);
        }
    }
}

void putPixelOnScreen(Screen *ptr, short x, short y, unsigned char r, unsigned char g, unsigned char b)
{
    if (!ptr) return;

    ptr->buffer[y * ptr->width * 3 + x * 3] = r;
    ptr->buffer[y * ptr->width * 3 + x * 3 + 1] = g;
    ptr->buffer[y * ptr->width * 3 + x * 3 + 2] = b;
}


