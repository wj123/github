========================================================================================
1、Mdss_mdp.c (kernel\drivers\video\msm\mdss)
node point:
/sys/devices/fd900000.qcom,mdss_mdp
root@SSS999ZZ:/sys/devices/fd900000.qcom,mdss_mdp # ls -l
-r--r--r-- root     root         4096 1970-01-01 00:14 caps
lrwxrwxrwx root     root              1970-01-01 00:14 driver -> ../../bus/platform/drivers/mdp
-r--r--r-- root     root         4096 1970-01-01 00:14 microamps_requested_fd900000.qcom,mdss_mdp-vdd
-r--r--r-- root     root         4096 1970-01-01 00:14 microamps_requested_fd900000.qcom,mdss_mdp-vdd-cx
-r--r--r-- root     root         4096 1970-01-01 00:14 modalias
drwxr-xr-x root     root              1970-01-01 00:14 power
drwxr-xr-x root     root              1970-01-01 00:14 qcom,mdss_fb_primary.152
drwxr-xr-x root     root              1970-01-01 00:14 qcom,mdss_fb_wfd.153
lrwxrwxrwx root     root              1970-01-01 00:14 subsystem -> ../../bus/platform
-rw-r--r-- root     root         4096 1970-01-01 00:14 uevent

logs:
<4>[    0.863183] [lzl-test]MDP HW Base phy_Address=0xfd900000 virt=0xc82c0000
<4>[    0.863220] [lzl-test]MDSS VBIF HW Base phy_Address=0xfd924000 virt=0xc8142000
<4>[    0.863243] [lzl-test]MDSS irq 104
<4>[    0.863458] [lzl-test]mdss_mdp_parse_bootarg: cmd_line =androidboot.hardware=qcom user_debug=31 msm_rtb.filter=0x37 androidboot.serialno=004401115143568 androidboot.wakeinfo=65535 androidboot.pmicinfo=1 androidboot.colorver=00 androidboot.recovery=0 androidboot.fotainfo=0 androidboot.emmc=true androidboot.baseband=msm mdss_mdp.panel=0:dsi:0:
<4>[    0.863531] [lzl-test]mdss_mdp_parse_bootarg: disp_idx =mdss_mdp.panel=0:dsi:0:
<4>[    0.863559] [lzl-test]mdss_mdp_parse_bootarg:1458: pan_name=[0:dsi:0:] end=[(null)]
<4>[    0.863583] [lzl-test]mdss_mdp_parse_bootarg:1463: pan_name=[0:dsi:0:] end=[]
<4>[    0.863604] [lzl-test]mdss_mdp_parse_bootarg:1484 panel:[0:dsi:0:]
	              [lzl-test]mdss_mdp_get_pan_cfg:pan_name = dsi:0:
<4>[    0.863638] [lzl-test]mdss_mdp_get_pan_cfg:1367 panel intf dsi
<4>[    0.863657] [lzl-test]mdss_mdp_get_pan_cfg:1371: t=[0:] panel name=[0:]
<4>[    0.863677] [lzl-test]mdss_mdp_get_pan_cfg:1383: pan_cfg->pan_intf[0]
<4>[    0.863697] [lzl-test]max mdp clk rate=200000000
<4>[    0.864007] [lzl-test]mdp clk rate=200000000		
========================================================================================
/android/kernel/drivers/video/msm/mdss/Makefile
驱动的注册流程：
 |------------------------------|     |----------------------------------------|         |--------------------------------------|        |--------------------------------------|
 |            (mdp)             |     |             (dsi)                      |         |         (primary fb)                 |        |         (lcd-backlight)              |  
 |                              |     |                                        |         |                                      |        |                                      |
 | device node:                 |     | device node:                           |         |  device node:                        |        |  device node:                        |
 | /sys/devices/                |     | /sys/devices/                          |         |  /sys/devices/fd900000.qcom,mdss_mdp/|        |  /sys/devices/fd900000.qcom,mdss_mdp |
 | fd900000.qcom,mdss_mdp       |     | fd922800.qcom,mdss_dsi                 |         |  qcom,mdss_fb_primary.152            |        |  /qcom,mdss_fb_primary.152/leds/     |      
 |                              |     |                                        |         |                                      |        |  lcd-backlight                       |            
 | driver node:                 |     | driver node:                           |         |  driver node:                        |        |                                      |
 | sys/bus/platform/drivers/mdp |---->| sys/bus/platform/drivers/mdss_dsi_ctrl |-------->|  sys/bus/platform/drivers/mdss_fb    |------->|  /sys/class/leds/lcd-backlight       |
 |                              |     |                                        |         |                                      |        |                                      |
 | child node:                  |     |                                        |         |                                      |        |                                      |
 | qcom,mdss_fb_primary.152     |     |                                        |         |                                      |        |                                      |
 | qcom,mdss_fb_wfd.153         |     |                                        |         |                                      |        |                                      |
 |------------------------------|     |----------------------------------------|         |--------------------------------------|        |--------------------------------------|

static const struct of_device_id mdss_mdp_dt_match[] = {
    { .compatible = "qcom,mdss_mdp",},
    {}
};
MODULE_DEVICE_TABLE(of, mdss_mdp_dt_match);

static struct platform_driver mdss_mdp_driver = {
    .probe = mdss_mdp_probe,
    .remove = mdss_mdp_remove,
    .suspend = mdss_mdp_suspend,//nothing
    .resume = mdss_mdp_resume,  //nothing
    .shutdown = NULL,
    .driver = {
        .name = "mdp",
        .of_match_table = mdss_mdp_dt_match,
        .pm = &mdss_mdp_pm_ops,
    },
};
static const struct dev_pm_ops mdss_mdp_pm_ops = {
    SET_SYSTEM_SLEEP_PM_OPS(mdss_mdp_pm_suspend, mdss_mdp_pm_resume)
    SET_RUNTIME_PM_OPS(mdss_mdp_runtime_suspend,
            mdss_mdp_runtime_resume,
            mdss_mdp_runtime_idle)
};
#define SET_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
    .suspend = suspend_fn, \    //mdss_mdp_pm_suspend
    .resume = resume_fn, \      //mdss_mdp_pm_resume
    .freeze = suspend_fn, \     //mdss_mdp_pm_suspend
    .thaw = resume_fn, \        //mdss_mdp_pm_resume
    .poweroff = suspend_fn, \   //mdss_mdp_pm_suspend
    .restore = resume_fn,       //mdss_mdp_pm_resume

#define SET_RUNTIME_PM_OPS(suspend_fn, resume_fn, idle_fn) \
    .runtime_suspend = suspend_fn, \//mdss_mdp_runtime_suspend
    .runtime_resume = resume_fn, \  //mdss_mdp_runtime_resume
    .runtime_idle = idle_fn,        //mdss_mdp_runtime_idle

=======================================================================================
mdss_mdp_probe函数的主要功能：
     1.1、为关键驱动数据结构 struct mdss_data_type *mdata 分配空间
     1.2、获得 struct resource 资源  //【IORESOURCE_MEM】【IORESOURCE_IRQ】
     1.3、解释dts为 struct mdss_data_type *mdata 赋值 //mdss_mdp_parse_dt()
     1.4、clk和irq设置、ionclient创建，iommu初始化  //mdss_mdp_res_init(mdata);
     1.5、创建属性文件、挂接中断处理函数
----------------------------------------------------------------------------------------
1、dmss_mdp驱动的mdss_mdp_probe函数分析（删除了一些无关紧要的代码）：
========================================================================================
static int mdss_mdp_probe(struct platform_device *pdev)
{
    struct mdss_data_type *mdata;
    mdata = devm_kzalloc(&pdev->dev, sizeof(*mdata), GFP_KERNEL);
1：获得资源，赋值给mdata结构体
    解释如下项：
    ----------start---------
        reg = <0xfd900000 0x22100 0xfd924000 0x1000>;
        reg-names = "mdp_phys", "vbif_phys";
        interrupts = <0x0 0x48 0x0>;
     ----------end---------
    mdata->mdp_reg_size = resource_size(res);//"mdp_phys"
    mdata->mdp_base = devm_ioremap(&pdev->dev, res->start, mdata->mdp_reg_size);//"mdp_phys"    | struct mdss_hw mdss_mdp_hw = {
    mdata->vbif_base = devm_ioremap(&pdev->dev, res->start, resource_size(res));//"vbif_phys"   |   .hw_ndx = MDSS_HW_MDP,
    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);                                       |   .ptr = NULL,
    mdata->irq = res->start;                                                                    |   .irq_handler = mdss_mdp_isr,
    mdss_mdp_hw.ptr = mdata;                                                                    | };
2：解释msm8226-mdss.dtsi文件mdss_mdp: qcom,mdss_mdp@fd900000 node
    mdss_mdp_parse_dt(pdev);
        2.1：mdss_mdp_parse_dt_hw_settings(pdev);
            解释如下两项： struct mdss_hw_settings *hws;
            ----------start---------
            qcom,vbif-settings = <0x4 0x1 0xd8 0x707 0x124 0x3>;24 0x00000003>;
            qcom,mdp-settings = <0x2e0 0xa5 0x2e4 0x55>;
            ----------end---------
                mdata->hw_settings = hws

        2.2：mdss_mdp_parse_dt_pipe(pdev);
            解释如下六项： struct mdss_mdp_pipe *head；
            ----------start---------
            qcom,mdss-pipe-vig-off = <0x00001200>;
            qcom,mdss-pipe-rgb-off = <0x00001E00>;
            qcom,mdss-pipe-dma-off = <0x00002A00>;
            qcom,mdss-pipe-vig-fetch-id = <1>;
            qcom,mdss-pipe-rgb-fetch-id = <7>;
            qcom,mdss-pipe-dma-fetch-id = <4>;
            ----------end ---------
            for (i = 0; i < len; i++) {
                head[i].type = type;//MDSS_MDP_PIPE_TYPE_VIG MDSS_MDP_PIPE_TYPE_RGB MDSS_MDP_PIPE_TYPE_DMA
                head[i].ftch_id  = ftch_id[i];//1 7 4
                head[i].num = i + num_base; //MDSS_MDP_SSPP_VIG0 MDSS_MDP_SSPP_RGB0 MDSS_MDP_SSPP_DMA0
                head[i].ndx = BIT(i + num_base);
                head[i].base = mdata->mdp_base + offsets[i];
            }
        2.3：mdss_mdp_parse_dt_mixer(pdev);
            解释如下四项： struct mdss_mdp_mixer *head；
            ----------start---------
            qcom,mdss-mixer-intf-off = <0x00003200>;
            qcom,mdss-mixer-wb-off = <0x00003E00>;
            qcom,mdss-dspp-off = <0x00004600>;
            qcom,mdss-pingpong-off = <0x00021B00>;
            ----------end------------
            mdss_mdp_mixer_addr_setup(mdata, mixer_offsets,
                                    dspp_offsets, pingpong_offsets,
                                    MDSS_MDP_MIXER_TYPE_INTF, mdata->nmixers_intf);

            mdss_mdp_mixer_addr_setup(mdata, mixer_offsets +
                                    mdata->nmixers_intf, NULL, NULL,
                                    MDSS_MDP_MIXER_TYPE_WRITEBACK, mdata->nmixers_wb);
                    分配struct mdss_mdp_mixer *head；
                    for (i = 0; i < len; i++) {
                        head[i].type = type;
                        head[i].base = mdata->mdp_base + mixer_offsets[i];
                        head[i].ref_cnt = 0;
                        head[i].num = i;
                        if (type == MDSS_MDP_MIXER_TYPE_INTF) {
                            head[i].dspp_base = mdata->mdp_base + dspp_offsets[i];
                            head[i].pingpong_base = mdata->mdp_base +
                                pingpong_offsets[i];
                        }
                    }
                    if ((type == MDSS_MDP_MIXER_TYPE_WRITEBACK) && !mdata->has_wfd_blk)
                        head[len] = head[len - 1];
                    case MDSS_MDP_MIXER_TYPE_INTF:
                        mdata->mixer_intf = head;
                    case MDSS_MDP_MIXER_TYPE_WRITEBACK:
                        mdata->mixer_wb = head;

        2.4：mdss_mdp_parse_dt_ctl(pdev);
            解释如下两项： struct mdss_mdp_ctl *head;
            ----------start---------
            qcom,mdss-ctl-off = <0x00000600 0x00000700>;
            qcom,mdss-wb-off = <0x00011100 0x00013100>;
            ----------end ---------
            for (i = 0; i < len; i++) {
                head[i].num = i;
                head[i].base = (mdata->mdp_base) + ctl_offsets[i];
                head[i].wb_base = (mdata->mdp_base) + wb_offsets[i];
                head[i].ref_cnt = 0;
            }

            if (!mdata->has_wfd_blk) {
                head[len - 1].shared_lock = shared_lock;
                head[len] = head[len - 1];
                head[len].num = head[len - 1].num;
            }
            mdata->ctl_off = head;

        2.5：mdss_mdp_parse_dt_video_intf(pdev);
            解释如下一项： struct mdss_mdp_video_ctx *head；
            ----------start---------
            qcom,mdss-intf-off = <0x00000000 0x00021300>;
            ----------end---------
            for (i = 0; i < count; i++) {
                head[i].base = mdata->mdp_base + offsets[i];
                head[i].ref_cnt = 0;
                head[i].intf_num = i + MDSS_MDP_INTF0;
                INIT_LIST_HEAD(&head[i].vsync_handlers);
            }
            mdata->video_intf = head;
            mdata->nintf = count;

        2.6：mdss_mdp_parse_dt_smp(pdev);
            解释如下三项： struct mdss_mdp_video_ctx *head；
            ----------start---------
            qcom,mdss-smp-data = <7 4096>;
            qcom,mdss-smp-mb-per-pipe = <4>;
            qcom,mdss-pipe-rgb-fixed-mmb = //没有在dts中定义
            ----------end ---------
            mdata->smp_mb_cnt = 4;
            mdata->smp_mb_size = 4096;
            mdata->smp_mb_per_pipe=4；

        2.7：mdss_mdp_parse_dt_misc(pdev);
            解释如下五项：
            ----------start---------
            qcom,mdss-rot-block-size = <64>;
            qcom,mdss-has-bwc = //没有在dts中定义
            qcom,mdss-has-decimation = //没有在dts中定义
            qcom,mdss-has-wfd-blk = //没有在dts中定义
            batfet-supply =
            ----------start---------
            mdata->rot_block_size = 64；
            mdata->has_bwc = false;
            mdata->has_decimation = false;
            mdata->has_wfd_blk = false;
            mdata->batfet_required = false;

        2.8：mdss_mdp_parse_dt_ad_cfg(pdev);
            解释如下五项：
            ----------start---------
            qcom,mdss-ad-off = //没有在dts中定义
            qcom,mdss-has-wb-ad = //没有在dts中定义
            ----------end---------
            mdata->nad_cfgs = mdss_mdp_parse_dt_prop_len(pdev, "qcom,mdss-ad-off");

            if (mdata->nad_cfgs == 0) {
                mdata->ad_cfgs = NULL;
                return 0;
            }

            2.9：mdss_mdp_parse_bootarg(pdev);
            //#cat /proc/cmdline                                                                   |struct mdss_panel_cfg *pan_cfg;
            // androidboot.hardware=qcom user_debug=31 msm_rtb.filter=0x37 androidboot.emmc=true   |struct mdss_panel_cfg {
            // androidboot.serialno=1a180627 androidboot.baseband=msm mdss_mdp.panel=0:dsi:0:      |    char arg_cfg[MDSS_MAX_PANEL_LEN + 1];
                chosen_node = of_find_node_by_name(NULL, "chosen");                                |    int  pan_intf;
                cmd_line = of_get_property(chosen_node, "bootargs", &len);                         |    bool lk_cfg;
                                                                                                   |    bool init_done;
                                                                                                   |};
                mdss_mdp_get_pan_cfg()                                                             |
                    pan_cfg->lk_cfg = false;                                                       |panel_name = &pan_cfg->arg_cfg[0];
                    pan_cfg->arg_cfg = '0:'                                                        |intf_type = &pan_cfg->pan_intf;
                    pan_cfg->pan_intf = MDSS_PANEL_INTF_DSI；
                pan_cfg->init_done = true;
                //mdss_mdp_parse_bootarg:1464 panel:[0:dsi:0:][lzl-test]mdss_mdp_get_pan_cfg:1350 panel intf dsi

3：clk和irq设置，ionclient创建，iommu初始化
    mdss_mdp_res_init(mdata);
            mdata->res_init = true;
            mdata->clk_ena = false;
            mdata->irq_mask = MDSS_MDP_DEFAULT_INTR_MASK;
            mdata->irq_ena = false;
            3.1：mdss_mdp_irq_clk_setup(mdata);
                解释如下五项：
                ----------start---------
                qcom,max-clk-rate = <200000000>;
                vdd-supply = <&gdsc_mdss>;
                vdd-cx-supply = <&pm8226_s1_corner>;

                gdsc_mdss: qcom,gdsc@fd8c2304 {
                    compatible = "qcom,gdsc";
                    regulator-name = "gdsc_mdss";
                    reg = <0xfd8c2304 0x4>;
                    status = "ok";
                    qcom,clock-names = "core_clk", "lut_clk";
                    linux,phandle = <0xe>;
                    phandle = <0xe>;
                };
                pm8226_s1_corner: regulator-s1-corner {
                    compatible = "qcom,rpm-regulator-smd";
                    regulator-name = "8226_s1_corner";
                    qcom,set = <0x3>;
                    regulator-min-microvolt = <0x1>;
                    regulator-max-microvolt = <0x7>;
                    qcom,use-voltage-corner;
                    qcom,consumer-supplies = "vdd_dig", "";
                    linux,phandle = <0x1b>;
                    phandle = <0x1b>;
                };
                ---------- end ---------
                mdata->max_mdp_clk_rate = 200000000；
                devm_request_irq(&mdata->pdev->dev, mdata->irq, mdss_irq_handler,IRQF_DISABLED, "MDSS", mdata);//注册中断，中断总入口
                mdata->fs = devm_regulator_get(&mdata->pdev->dev, "vdd");
                mdata->fs_ena = false;
                mdata->vdd_cx = devm_regulator_get(&mdata->pdev->dev,"vdd-cx");
                mdss_mdp_irq_clk_register(mdata, "bus_clk", MDSS_CLK_AXI)
                mdss_mdp_irq_clk_register(mdata, "iface_clk", MDSS_CLK_AHB)
                mdss_mdp_irq_clk_register(mdata, "core_clk_src", MDSS_CLK_MDP_SRC)
                mdss_mdp_irq_clk_register(mdata, "core_clk", MDSS_CLK_MDP_CORE)
                mdss_mdp_irq_clk_register(mdata, "lut_clk", MDSS_CLK_MDP_LUT)
                mdss_mdp_irq_clk_register(mdata, "vsync_clk", MDSS_CLK_MDP_VSYNC)
                mdss_mdp_set_clk_rate(MDP_CLK_DEFAULT_RATE);

            mdata->iclient = msm_ion_client_create(-1, mdata->pdev->name);//创建ion_client
            mdss_iommu_init(mdata);

    3.1：mdss_mdp_pp_init(&pdev->dev);
    3.2：mdss_mdp_bus_scale_register(mdata);
    3.3：mdss_mdp_bus_scale_set_quota(AB_QUOTA, IB_QUOTA);
    3.4：mdss_mdp_debug_init(mdata);  //创建debug

    pm_runtime_set_suspended(&pdev->dev);
    pm_runtime_enable(&pdev->dev);
    if (!pm_runtime_enabled(&pdev->dev))
        mdss_mdp_footswitch_ctrl(mdata, true);

    mdss_mdp_register_sysfs(mdata);//在mdp目录下，创建一个caps属性文件
        -------- start -----
        #cat /sys/devices/fd900000.qcom,mdss_mdp/caps
            mdp_version=5 hw_rev=268500993
            rgb_pipes=1
            vig_pipes=1                             |-------------------------------------------------------------|
            dma_pipes=1              |------------>>|struct msm_mdp_interface mdp5 = {                            |
            smp_count=7              |              |    .init_fnc = mdss_mdp_overlay_init,                       |
            smp_size=4096            |              |    .fb_mem_get_iommu_domain = mdss_fb_mem_get_iommu_domain, |
            max downscale ratio=4    |              |    .panel_register_done = mdss_panel_register_done,         |
            max upscale ratio=20     |              |    .fb_stride = mdss_mdp_fb_stride,                         |
            features                 |              |};                                                           |
        ------- end --------         |              |-------------------------------------------------------------|
    mdss_fb_register_mdp_instance(&mdp5); //mdp_instance = mdp5; //只是一个简单的赋值操作
    mdss_register_irq(&mdss_mdp_hw);
    |---------------------------------|mdss_register_irq（&mdss_mdp_hw）函数的功能是：
    | struct mdss_hw mdss_mdp_hw = {  |mdss_irq_handlers[hw->hw_ndx] = mdss_mdp_hw;
    |   .hw_ndx = MDSS_HW_MDP,        |
    |   .ptr = mdata,                 |
    |   .irq_handler = mdss_mdp_isr,  |
    | };                              |
    |---------------------------------|
}


====================================================================================================
mfd->mdp的接口函数
====================================================================================================
struct msm_mdp_interface {
    int (*fb_mem_alloc_fnc)(struct msm_fb_data_type *mfd);      // null
    int (*fb_mem_get_iommu_domain)(void);                       // mdss_fb_mem_get_iommu_domain
    int (*init_fnc)(struct msm_fb_data_type *mfd);              // mdss_mdp_overlay_init
    int (*on_fnc)(struct msm_fb_data_type *mfd);                // mdss_mdp_overlay_on
    int (*off_fnc)(struct msm_fb_data_type *mfd);               // mdss_mdp_overlay_off
    int (*release_fnc)(struct msm_fb_data_type *mfd);           // __mdss_mdp_overlay_release_all
    int (*kickoff_fnc)(struct msm_fb_data_type *mfd, struct mdp_display_commit *data); // mdss_mdp_overlay_kickoff
    int (*ioctl_handler)(struct msm_fb_data_type *mfd, u32 cmd, void *arg);             // mdss_mdp_overlay_ioctl_handler
    void (*dma_fnc)(struct msm_fb_data_type *mfd);                                      // mdss_mdp_overlay_pan_display
    int (*cursor_update)(struct msm_fb_data_type *mfd, struct fb_cursor *cursor);       // mdss_mdp_hw_cursor_update
    int (*lut_update)(struct msm_fb_data_type *mfd, struct fb_cmap *cmap);              // NULL
    int (*do_histogram)(struct msm_fb_data_type *mfd, struct mdp_histogram *hist);      // NULL
    int (*update_ad_input)(struct msm_fb_data_type *mfd);           // NULL
    int (*panel_register_done)(struct mdss_panel_data *pdata);      // mdss_panel_register_done
    u32 (*fb_stride)(u32 fb_index, u32 xres, int bpp);              // mdss_mdp_fb_stride
    struct msm_sync_pt_data *(*get_sync_fnc)                        // mdss_mdp_rotator_sync_pt_get
        (struct msm_fb_data_type *mfd, const struct mdp_buf_sync *buf_sync);
    void *private1;
};

struct msm_mdp_interface mdp5 = {
    .init_fnc = mdss_mdp_overlay_init,
    .fb_mem_get_iommu_domain = mdss_fb_mem_get_iommu_domain,
    .panel_register_done = mdss_panel_register_done,
    .fb_stride = mdss_mdp_fb_stride,
};
在mdss_mdp_probe()的probe函数中调用如下函数：
    rc = mdss_fb_register_mdp_instance(&mdp5);
函数的原型如下：
【 mdss_fb.c (kernel\drivers\video\msm\mdss)  】
static struct msm_mdp_interface *mdp_instance; //声明
int mdss_fb_register_mdp_instance(struct msm_mdp_interface *mdp)
{...............
    mdp_instance = mdp; //只是一个简单的赋值操作
    return 0;
}
EXPORT_SYMBOL(mdss_fb_register_mdp_instance);

在mdss_fb_probe()的probe函数中
    mfd->mdp = *mdp_instance; //将mdp_instance的所有内容拷贝一份给 mfd->mdp

所以mfd->mdp等于mdp5，mdp_instance指向mdp5                                          |--------------------------------------------------------
                    |----------------------------------------------------------|    |struct mdss_overlay_private {
                    |.fb_mem_alloc_fnc = [没有定义]                              |    |   ktime_t vsync_time;
mfd->mdp   ==   mdp5|.init_fnc = mdss_mdp_overlay_init,                        |    |   struct sysfs_dirent *vsync_event_sd;
mdp_instance -->mdp5|.fb_mem_get_iommu_domain = mdss_fb_mem_get_iommu_domain,  |    |   int borderfill_enable;
                    |.panel_register_done = mdss_panel_register_done,          |    |   int overlay_play_enable;
                    |.fb_stride = mdss_mdp_fb_stride,                          |    |   int hw_refresh;
                    |.on_fnc = mdss_mdp_overlay_on;                            |    |   void *cpu_pm_hdl;
                    |.off_fnc = mdss_mdp_overlay_off;                          |    |
                    |.release_fnc = __mdss_mdp_overlay_release_all;            |    |   struct mdss_data_type *mdata;
                    |.do_histogram = NULL;                                     |    |   struct mutex ov_lock;
                    |.cursor_update = mdss_mdp_hw_cursor_update;               |    |   struct mdss_mdp_ctl *ctl;
                    |.dma_fnc = mdss_mdp_overlay_pan_display;                  |    |   struct mdss_mdp_wb *wb;
                    |.ioctl_handler = mdss_mdp_overlay_ioctl_handler;          |    |   struct list_head overlay_list;
                    |.panel_register_done = mdss_panel_register_done;          |    |   struct list_head pipes_used;
                    |.kickoff_fnc = mdss_mdp_overlay_kickoff;                  |    |   struct list_head pipes_cleanup;
                    |.get_sync_fnc = mdss_mdp_rotator_sync_pt_get;             |    |   struct list_head rot_proc_list;
                    |.lut_update = [没有定义]                                    |    |   bool mixer_swap;
                    |.update_ad_input = [没有定义 ]                              |    |
                    |.private1 ---------------------------------------------------> |   struct mdss_mdp_data free_list[MAX_FREE_LIST_SIZE];
                    |----------------------------------------------------------|    |   int free_list_size;
                                                                                    |   int ad_state;
                                                                                    |
                                                                                    |   bool handoff;
【 mdss_mdp_overlay.c (kernel\drivers\video\msm\mdss) 】                            |   u32 splash_mem_addr;
int mdss_mdp_overlay_init(struct msm_fb_data_type *mfd)                             |   u32 splash_mem_size;
{                                                                                   |   u32 sd_enabled;
    struct device *dev = mfd->fbi->dev;                                             |};
    struct msm_mdp_interface *mdp5_interface = &mfd->mdp;                           |----------------------------------------------------------
    struct mdss_overlay_private *mdp5_data = NULL;

    mdp5_interface->on_fnc = mdss_mdp_overlay_on;
    mdp5_interface->off_fnc = mdss_mdp_overlay_off;
    mdp5_interface->release_fnc = __mdss_mdp_overlay_release_all;
    mdp5_interface->do_histogram = NULL;
    mdp5_interface->cursor_update = mdss_mdp_hw_cursor_update;
    mdp5_interface->dma_fnc = mdss_mdp_overlay_pan_display;
    mdp5_interface->ioctl_handler = mdss_mdp_overlay_ioctl_handler;
    mdp5_interface->panel_register_done = mdss_panel_register_done;
    mdp5_interface->kickoff_fnc = mdss_mdp_overlay_kickoff;
    mdp5_interface->get_sync_fnc = mdss_mdp_rotator_sync_pt_get;

    mdp5_data = kmalloc(sizeof(struct mdss_overlay_private), GFP_KERNEL);
    INIT_LIST_HEAD(&mdp5_data->pipes_used);
    INIT_LIST_HEAD(&mdp5_data->pipes_cleanup);
    INIT_LIST_HEAD(&mdp5_data->rot_proc_list);
    mutex_init(&mdp5_data->ov_lock);
    mdp5_data->hw_refresh = true;
    mdp5_data->overlay_play_enable = true;
    mdp5_data->mdata = dev_get_drvdata(mfd->pdev->dev.parent);
    mfd->mdp.private1 = mdp5_data;

    rc = mdss_mdp_overlay_fb_parse_dt(mfd);
        解释如下项：
        ----- start -----
        qcom,mdss-mixer-swap = //没有定义
        qcom,memblock-reserve = <0x3200000 0xfa0000>;
        -----  enf -----
        mdp5_mdata->mixer_swap = //没有定义
        mdp5_mdata->splash_mem_addr = offsets[0]; // 0x3200000
        mdp5_mdata->splash_mem_size = offsets[1]; // 0xfa0000
    //在/sys/class/graphics/fb0/创建 vsync_event和ad属性文件
    rc = sysfs_create_group(&dev->kobj, &mdp_overlay_sysfs_group);
    mdp5_data->vsync_event_sd = sysfs_get_dirent(dev->kobj.sd, NULL,"vsync_event");
    // 在/sys/class/graphics/fb0/创建 dynamic_fps 属性文件
    rc = sysfs_create_group(&dev->kobj,&dynamic_fps_fs_attrs_group);
    mfd->mdp_sync_pt_data.async_wait_fences = true;
    //在/sys/class/graphics/fb0/创建 mdp -> sys/devices/fd900000.qcom,mdss_mdp
    rc = sysfs_create_link_nowarn(&dev->kobj, &mdp5_data->mdata->pdev->dev.kobj, "mdp");
    kobject_uevent(&dev->kobj, KOBJ_ADD);
    mdp5_data->cpu_pm_hdl = add_event_timer(NULL, (void *)mdp5_data);//目前是空函数，因为没有定义CONFIG_MSM_EVENT_TIMER 配置宏
    ......
}

