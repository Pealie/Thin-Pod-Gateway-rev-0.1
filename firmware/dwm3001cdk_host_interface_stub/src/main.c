#include <errno.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#if !DT_NODE_EXISTS(DT_ALIAS(tphost_sck))
#error "Missing devicetree alias tphost-sck"
#endif

#if !DT_NODE_EXISTS(DT_ALIAS(tphost_mosi))
#error "Missing devicetree alias tphost-mosi"
#endif

#if !DT_NODE_EXISTS(DT_ALIAS(tphost_miso))
#error "Missing devicetree alias tphost-miso"
#endif

#if !DT_NODE_EXISTS(DT_ALIAS(tphost_cs))
#error "Missing devicetree alias tphost-cs"
#endif

#if !DT_NODE_EXISTS(DT_ALIAS(tphost_ready))
#error "Missing devicetree alias tphost-ready"
#endif

#define HEARTBEAT_MS 500

static const struct gpio_dt_spec host_sck = GPIO_DT_SPEC_GET(DT_ALIAS(tphost_sck), gpios);
static const struct gpio_dt_spec host_mosi = GPIO_DT_SPEC_GET(DT_ALIAS(tphost_mosi), gpios);
static const struct gpio_dt_spec host_miso = GPIO_DT_SPEC_GET(DT_ALIAS(tphost_miso), gpios);
static const struct gpio_dt_spec host_cs = GPIO_DT_SPEC_GET(DT_ALIAS(tphost_cs), gpios);
static const struct gpio_dt_spec host_ready = GPIO_DT_SPEC_GET(DT_ALIAS(tphost_ready), gpios);

static int configure_input_pin(const struct gpio_dt_spec *pin, const char *name)
{
    int ret;

    if (!gpio_is_ready_dt(pin)) {
        printk("ERROR: %s GPIO controller is not ready\n", name);
        return -ENODEV;
    }

    ret = gpio_pin_configure_dt(pin, GPIO_INPUT);
    printk("pin_config name=%s port=%s pin=%u mode=input ret=%d\n",
           name, pin->port->name, (unsigned int)pin->pin, ret);

    return ret;
}

static int configure_ready_pin(void)
{
    int ret;

    if (!gpio_is_ready_dt(&host_ready)) {
        printk("ERROR: HOST_READY GPIO controller is not ready\n");
        return -ENODEV;
    }

    ret = gpio_pin_configure_dt(&host_ready, GPIO_OUTPUT_INACTIVE | GPIO_INPUT);
    printk("pin_config name=HOST_READY port=%s pin=%u mode=output_input_inactive ret=%d\n",
           host_ready.port->name, (unsigned int)host_ready.pin, ret);

    return ret;
}

int main(void)
{
    uint32_t heartbeat = 0;
    int ret;

    printk("\nThin-Pod DWM3001-CDK host-interface stub\n");
    printk("stage=boot_identity_ready_toggle\n");
    printk("board=decawave_dwm3001cdk\n");
    printk("role=gateway_initiator backend=stub state=pre_spis\n");
    printk("build_date=%s build_time=%s\n", __DATE__, __TIME__);
    printk("j10_pin_map sck=P0.31 mosi=P0.27 miso=P0.07 cs=P0.30 ready=P0.28 reset=P0.18_external\n");
    printk("purpose=prove DWM-side firmware owns host-interface pin definitions before SPIS proof\n");

    ret = configure_input_pin(&host_sck, "HOST_SCK_P0_31");
    if (ret != 0) {
        return ret;
    }

    ret = configure_input_pin(&host_mosi, "HOST_MOSI_P0_27");
    if (ret != 0) {
        return ret;
    }

    ret = configure_input_pin(&host_miso, "HOST_MISO_P0_07");
    if (ret != 0) {
        return ret;
    }

    ret = configure_input_pin(&host_cs, "HOST_CS_P0_30");
    if (ret != 0) {
        return ret;
    }

    ret = configure_ready_pin();
    if (ret != 0) {
        return ret;
    }

    while (1) {
        int set_ret;
        int ready_level;
        int cs_level;

        set_ret = gpio_pin_set_dt(&host_ready, 1);
        ready_level = gpio_pin_get_dt(&host_ready);
        cs_level = gpio_pin_get_dt(&host_cs);

        printk("host_if_stub heartbeat=%u phase=ready_high set_ret=%d ready=%d cs=%d role=gateway_initiator backend=stub state=pre_spis\n",
               heartbeat, set_ret, ready_level, cs_level);

        k_msleep(HEARTBEAT_MS);

        set_ret = gpio_pin_set_dt(&host_ready, 0);
        ready_level = gpio_pin_get_dt(&host_ready);
        cs_level = gpio_pin_get_dt(&host_cs);

        printk("host_if_stub heartbeat=%u phase=ready_low set_ret=%d ready=%d cs=%d role=gateway_initiator backend=stub state=pre_spis\n",
               heartbeat, set_ret, ready_level, cs_level);

        heartbeat++;
        k_msleep(HEARTBEAT_MS);
    }
    return 0;
}
