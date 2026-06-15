#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
    int tick = 1;

    printk("\n\nThin-Pod Node CDK alive\n");

    while (1) {
        printk("node tick %d\n", tick++);
        k_sleep(K_SECONDS(1));
    }

    return 0;
}
