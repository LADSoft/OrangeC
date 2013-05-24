/*
 * this example shows how to intercept extended (non-ASCII) keys.
 * Essentially, getch() returns either an ASCII code for standard keys
 * or a scancode * 256 for other keys
 */
#define ESC 0x1b

main()
{
    printf("Press a function key, <ESC> to exit\n");
    while (1) {
        /* get key */
        int a = getch();

        /* quit if it is the escape key */
        if (a == ESC)
            break;

        if (a == 0)
        {
            //non-ascii char
            a = getch();
            /* now check for function keys */
            /* we are ignoring everything but function keys here */
            if (a >=0x3b) {
                a-=0x3b;
                if (a < 10)
                    printf("You pressed the F%d key \r",a+1);
            }
        }
    }
}