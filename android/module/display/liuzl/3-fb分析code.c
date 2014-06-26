========================================================================================
1、mdss_fb.c (kernel\drivers\video\msm\mdss)
device node point:
root@SSS999ZZ:/sys/devices/fd900000.qcom,mdss_mdp/qcom,mdss_fb_primary.152 # ls -l
lrwxrwxrwx root     root              1970-01-01 05:08 driver -> ../../../bus/platform/drivers/mdss_fb
drwxr-xr-x root     root              1970-01-01 05:08 leds
-r--r--r-- root     root         4096 1970-01-01 05:08 modalias
drwxr-xr-x root     root              1970-01-01 05:08 power
lrwxrwxrwx root     root              1970-01-01 05:08 subsystem -> ../../../bus/platform
-rw-r--r-- root     root         4096 1970-01-01 05:08 uevent

driver node point:
root@SSS999ZZ:/sys/bus/platform/drivers/mdss_fb # ls -l
--w------- root     root         4096 1970-01-01 05:08 bind
lrwxrwxrwx root     root              1970-01-01 05:08 qcom,mdss_fb_primary.152 -> ../../../../devices/fd900000.qcom,mdss_mdp/qcom,mdss_fb_primary.152
lrwxrwxrwx root     root              1970-01-01 05:08 qcom,mdss_fb_wfd.153 -> ../../../../devices/fd900000.qcom,mdss_mdp/qcom,mdss_fb_wfd.153
--w------- root     root         4096 1970-01-01 05:08 uevent
--w------- root     root         4096 1970-01-01 05:08 unbind

fb0 node point:
root@SSS999ZZ:/sys/class/graphics/fb0 # ls -l
-rw-rw-r-- root     root         4096 1970-01-01 05:08 ad
-rw-r--r-- root     root         4096 1970-01-01 05:08 bits_per_pixel
-rw-r--r-- root     root         4096 1970-01-01 05:08 blank
-rw-r--r-- root     root         4096 1970-01-01 05:08 console
-rw-r--r-- root     root         4096 1970-01-01 05:08 cursor
-r--r--r-- root     root         4096 1970-01-01 05:08 dev
-rw-r--r-- root     root         4096 1970-01-01 05:08 dynamic_fps
lrwxrwxrwx root     root              1970-01-01 05:08 mdp -> ../../../fd900000.qcom,mdss_mdp
-rw-r--r-- root     root         4096 1970-01-01 05:08 mode
-rw-r--r-- root     root         4096 1970-01-01 05:08 modes
-r--r--r-- root     root         4096 1970-01-01 05:08 msm_fb_split
-r--r--r-- root     root         4096 1970-01-01 05:08 msm_fb_type
-r--r--r-- root     root         4096 1970-01-01 05:08 name
-rw-r--r-- root     root         4096 1970-01-01 05:08 pan
drwxr-xr-x root     root              1970-01-01 05:08 power
-rw-r--r-- root     root         4096 1970-01-01 05:08 rotate
-r--r--r-- root     root         4096 1970-01-01 05:08 show_blank_event
-rw-r--r-- root     root         4096 1970-01-01 05:08 state
-r--r--r-- root     root         4096 1970-01-01 05:08 stride
lrwxrwxrwx root     root              1970-01-01 05:08 subsystem -> ../../../../class/graphics
-rw-r--r-- root     root         4096 1970-01-01 05:08 uevent
-rw-r--r-- root     root         4096 1970-01-01 05:08 virtual_size
-r--r--r-- root     root         4096 1970-01-01 05:08 vsync_event
========================================================================================
【 fbmem.c (kernel\drivers\video) 】
static int __init  fbmem_init(void)
{
    1.1、在/proc/下创建fb节点
    proc_create("fb", 0, NULL, &fb_proc_fops);
    1.2、创建字符设备fb
    //#define FB_MAJOR      29   /* /dev/fb* framebuffers */  android下在/dev/graphics/fb*
    register_chrdev(FB_MAJOR,"fb",&fb_fops) //file_operations----|
    1.3、在/sys/class/目录下创建graphics                         |
    fb_class = class_create(THIS_MODULE, "graphics");            |
    return 0;                                                    |
}                                                                |
static const struct file_operations fb_fops = {  <---------------|
    .owner =    THIS_MODULE,
    .read =     fb_read,
    .write =    fb_write,
    .unlocked_ioctl = fb_ioctl,
    .mmap =     fb_mmap,
    .open =     fb_open,
    .release =  fb_release,
    .llseek =   default_llseek,
};
---------------------------------------------------------------------------
static const struct dev_pm_ops mdss_fb_pm_ops = {
    SET_SYSTEM_SLEEP_PM_OPS(mdss_fb_pm_suspend, mdss_fb_pm_resume)
};
#define SET_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
    .suspend = suspend_fn, \    //mdss_fb_pm_suspend
    .resume = resume_fn, \      //mdss_fb_pm_resume
    .freeze = suspend_fn, \     //mdss_fb_pm_suspend
    .thaw = resume_fn, \        //mdss_fb_pm_resume
    .poweroff = suspend_fn, \   //mdss_fb_pm_suspend
    .restore = resume_fn,       //mdss_fb_pm_resume

static const struct of_device_id mdss_fb_dt_match[] = {
    { .compatible = "qcom,mdss-fb",},
    {}
};
EXPORT_COMPAT("qcom,mdss-fb");
static struct platform_driver mdss_fb_driver = {
    .probe = mdss_fb_probe,
    .remove = mdss_fb_remove,
    .suspend = mdss_fb_suspend, // null
    .resume = mdss_fb_resume,   // null
    .shutdown = mdss_fb_shutdown,
    .driver = {
        .name = "mdss_fb",
        .of_match_table = mdss_fb_dt_match,
        .pm = &mdss_fb_pm_ops,
    },
};
========================================================================================
mdss_fb_probe函数的主要功能：
	1.1、为关键结构体struct msm_fb_data_type *mfd和 struct fb_info *fbi 分配空间
	1.2、为mfd的部分成员赋初值
	1.3、mdss_fb_register(mfd);  //关键函数，在下面分析
		1.3.1、为fbi的fix和var成员赋值
		1.3.2、为fb0分配空间		  //mdss_fb_alloc_fbmem(mfd)
		1.3.3、初始化fbi->cmap		//fb_alloc_cmap(&fbi->cmap, 256, 0);
		1.3.4、register_framebuffer(fbi) //注册fbi，在/sys/class/graphics/目录下创建fb0，并创建相关属性
	1.4、mfd->mdp.init_fnc(mfd);//mdss_mdp_overlay_init(),初始化mfd->mdp结构体的成员
	1.5、led_classdev_register(&pdev->dev, &backlight_led) //lcd-backlight注册 
----------------------------------------------------------------------------------------
2、mdss_fb驱动的mdss_fb_probe（）函数分析（删除了一些无关紧要的代码）：
----------------------------------------------------------------------------------------
static int mdss_fb_probe(struct platform_device *pdev)
{
    struct msm_fb_data_type *mfd = NULL;
    struct mdss_panel_data *pdata;
    struct fb_info *fbi;
    int rc;
    //参考mdss_register_panel()函数  pdata = &(ctrl_pdata->panel_data)
    pdata = dev_get_platdata(&pdev->dev);
|-------------------------------------------------------------------------------------|
|struct fb_info *fbi------>|---->|--------------|                                     |
|                          |     |              |                                     |
|                          |     |       par -------|                                 |
|                          |     |              |   |                                 |
|                          |     |--------------|<--|<---msm_fb_data_type *mfd        |
|                          |     |              |                                     |
|                          |---------- fbi      |                                     |
|                                |              |                                     |
|                                |--------------|                                     |
|-------------------------------------------------------------------------------------|
    fbi = framebuffer_alloc(sizeof(struct msm_fb_data_type), NULL);// alloc framebuffer info + par data

    mfd = (struct msm_fb_data_type *)fbi->par;
    mfd->key = MFD_KEY;
    mfd->fbi = fbi;
    mfd->panel_info = &pdata->panel_info;
    mfd->panel.type = pdata->panel_info.type;   //panel_info.type == MIPI_VIDEO_PANEL
    mfd->panel.id = mfd->index;
    mfd->fb_page = MDSS_FB_NUM;     //2
    mfd->index = fbi_list_index;
    mfd->mdp_fb_page_protection = MDP_FB_PAGE_PROTECTION_WRITECOMBINE;

    mfd->ext_ad_ctrl = -1;
    mfd->bl_level = 0;
    mfd->bl_scale = 1024;
    mfd->bl_min_lvl = 30;
    mfd->fb_imgType = MDP_RGBA_8888;

    mfd->pdev = pdev;
    if (pdata->next)  //不成立，双屏split模式才成了
        mfd->split_display = true;
    mfd->mdp = *mdp_instance;   // mdp的接口函数拷贝一份到mfd->mdp
    INIT_LIST_HEAD(&mfd->proc_list);

    mutex_init(&mfd->lock);
    mutex_init(&mfd->bl_lock);
    fbi_list[fbi_list_index++] = fbi;
    platform_set_drvdata(pdev, mfd);

    rc = mdss_fb_register(mfd);  //关键函数，在下面分析

    if (mfd->mdp.init_fnc)
        rc = mfd->mdp.init_fnc(mfd);//mdss_mdp_overlay_init(),初始化mfd->mdp结构体的成员

    rc = pm_runtime_set_active(mfd->fbi->dev);
    pm_runtime_enable(mfd->fbi->dev);
                                                                    //lcd-backlight注册                                                  
    /* android supports only one lcd-backlight/lcd for now */       |--------------------------------------------------|
    if (!lcd_backlight_registered) {                                |static struct led_classdev backlight_led = {      |
        if (led_classdev_register(&pdev->dev, &backlight_led))----->|   .name           = "lcd-backlight",             |
            pr_err("led_classdev_register failed\n");               |   .brightness     = MDSS_MAX_BL_BRIGHTNESS,      |
        else                                                        |   .brightness_set = mdss_fb_set_bl_brightness,   |
            lcd_backlight_registered = 1;                           |};                                                |
    }                                                               |--------------------------------------------------|

    mdss_fb_create_sysfs(mfd);  // sys/class/graphics/fb0/目录下创建msm_fb_type、msm_fb_split、show_blank_event属性文件
    mdss_fb_send_panel_event(mfd, MDSS_EVENT_FB_REGISTERED, fbi);

    mfd->mdp_sync_pt_data.fence_name = "mdp-fence";
    if (mfd->mdp_sync_pt_data.timeline == NULL) {
        char timeline_name[16];
        snprintf(timeline_name, sizeof(timeline_name),
            "mdss_fb_%d", mfd->index);
         mfd->mdp_sync_pt_data.timeline =
                sw_sync_timeline_create(timeline_name);
        if (mfd->mdp_sync_pt_data.timeline == NULL) {
            pr_err("%s: cannot create time line", __func__);
            return -ENOMEM;
        }
        mfd->mdp_sync_pt_data.notifier.notifier_call =
            __mdss_fb_sync_buf_done_callback;
    }
    if ((mfd->panel.type == WRITEBACK_PANEL) ||
            (mfd->panel.type == MIPI_CMD_PANEL))
        mfd->mdp_sync_pt_data.threshold = 1;
    else
        mfd->mdp_sync_pt_data.threshold = 2;

    return rc;
}

========================================================================================
3、mdss_fb驱动的mdss_fb_register（）函数分析（删除了一些无关紧要的代码）：
----------------------------------------------------------------------------------------
static int mdss_fb_register(struct msm_fb_data_type *mfd)
{
    int bpp;
    struct mdss_panel_info *panel_info = mfd->panel_info;
    struct fb_info *fbi = mfd->fbi;
    struct fb_fix_screeninfo *fix;
    struct fb_var_screeninfo *var;

    fix = &fbi->fix;
    var = &fbi->var;

    fix->type_aux = 0;  /* if type == FB_TYPE_INTERLEAVED_PLANES */
    fix->visual = FB_VISUAL_TRUECOLOR;  /* True Color */
    fix->ywrapstep = 0; /* No support */
    fix->mmio_start = 0;    /* No MMIO Address */
    fix->mmio_len = 0;  /* No MMIO Address */
    fix->accel = FB_ACCEL_NONE;/* FB_ACCEL_MSM needes to be added in fb.h */

    var->xoffset = 0,   /* Offset from virtual to visible */
    var->yoffset = 0,   /* resolution */
    var->grayscale = 0, /* No graylevels */
    var->nonstd = 0,    /* standard pixel format */
    var->activate = FB_ACTIVATE_VBL,    /* activate it at vsync */
    var->height = -1,   /* height of picture in mm */
    var->width = -1,    /* width of picture in mm */
    var->accel_flags = 0,   /* acceleration flags */
    var->sync = 0,  /* see FB_SYNC_* */
    var->rotate = 0,    /* angle we rotate counter clockwise */
    mfd->op_enable = false;

    switch (mfd->fb_imgType) {// mfd->fb_imgType = MDP_RGBA_8888;
    case MDP_RGBA_8888:
        fix->type = FB_TYPE_PACKED_PIXELS;
        fix->xpanstep = 1;
        fix->ypanstep = 1;
        var->vmode = FB_VMODE_NONINTERLACED;
        var->blue.offset = 8;
        var->green.offset = 16;
        var->red.offset = 24;
        var->blue.length = 8;
        var->green.length = 8;
        var->red.length = 8;
        var->blue.msb_right = 0;
        var->green.msb_right = 0;
        var->red.msb_right = 0;
        var->transp.offset = 0;
        var->transp.length = 8;
        bpp = 4;
        break;
    }
    var->xres = panel_info->xres;
    fix->type = panel_info->is_3d_panel;
    if (mfd->mdp.fb_stride) //成立  mdss_mdp_fb_stride() ALIGN(xres, 32) * bpp; 函数的功能是让var->xres 向上32位对齐
        fix->line_length = mfd->mdp.fb_stride(mfd->index, var->xres, bpp);

    var->yres = panel_info->yres;
    if (panel_info->physical_width)
        var->width = panel_info->physical_width;
    if (panel_info->physical_height)
        var->height = panel_info->physical_height;
    var->xres_virtual = var->xres;
    var->yres_virtual = panel_info->yres * mfd->fb_page;
    var->bits_per_pixel = bpp * 8;  /* FrameBuffer color depth */
    var->upper_margin = panel_info->lcdc.v_back_porch;
    var->lower_margin = panel_info->lcdc.v_front_porch;
    var->vsync_len = panel_info->lcdc.v_pulse_width;
    var->left_margin = panel_info->lcdc.h_back_porch;
    var->right_margin = panel_info->lcdc.h_front_porch;
    var->hsync_len = panel_info->lcdc.h_pulse_width;
    var->pixclock = panel_info->clk_rate / 1000;
    |----------------------|  struct panel_id
    |struct panel_id {     |    |---------|
    |   u16 id;            |id  | 00 | 00 |
    |   u16 type;          |    |---------|
    |};                    |type| 00 | 08 |
    |----------------------|    |---------|
    //在mdss_fb_probe()函数中已经赋值为如下：
    //mfd->panel.type = pdata->panel_info.type;     //MIPI_VIDEO_PANEL = 8
    //mfd->panel.id = mfd->index;                   // 0
    id = (int *)&mfd->panel;
    // fix->id = "mdssfb_80000"
    snprintf(fix->id, sizeof(fix->id), "mdssfb_%x", (u32) *id);     |--------------------------------------------------------------|
                                                                    |static struct fb_ops mdss_fb_ops = {                          |
    fbi->fbops = &mdss_fb_ops;------------------------------------>>|   .owner = THIS_MODULE,                                      |
    fbi->flags = FBINFO_FLAG_DEFAULT;                               |   .fb_open = mdss_fb_open,                                   |
    fbi->pseudo_palette = mdss_fb_pseudo_palette;                   |   .fb_release = mdss_fb_release,                             |
                                                                    |   .fb_check_var = mdss_fb_check_var,  /* vinfo check */      |
    mfd->ref_cnt = 0;                                               |   .fb_set_par = mdss_fb_set_par,  /* set the video mode */   |
    mfd->panel_power_on = false;                                    |   .fb_blank = mdss_fb_blank,  /* blank display */            |
    mfd->dcm_state = DCM_UNINIT;                                    |   .fb_pan_display = mdss_fb_pan_display,  /* pan display */  |
                                                                    |   .fb_ioctl = mdss_fb_ioctl,  /* perform fb specific ioctl */|
    mdss_fb_parse_dt_split(mfd); 【双屏模式，不成立】               |   .fb_mmap = mdss_fb_mmap,
                                                                    |};|-----------------------------------------------------------|
    mdss_fb_alloc_fbmem(mfd) //为fb设备准备空间
        |-->dom = mfd->mdp.fb_mem_get_iommu_domain();//mdss_fb_mem_get_iommu_domain()
        |   |-->mdss_get_iommu_domain(MDSS_IOMMU_DOMAIN_UNSECURE);
        |       |-->return mdss_res->iommu_map[type].domain_idx;
        |-->mdss_fb_alloc_fbmem_iommu(mfd, dom);
            |-->of_property_read_u32(pdev->dev.of_node,"qcom,memory-reservation-size", &size)//qcom,memory-reservation-size = <0x800000>;
            |-->virt = allocate_contiguous_memory(size, MEMTYPE_EBI1, SZ_1M, 0);
            |-->phys = memory_pool_node_paddr(virt);
            |-->msm_iommu_map_contig_buffer(phys, dom, 0, size, SZ_4K, 0,&mfd->iova);
            |-->mfd->fbi->screen_base = virt;
            |-->mfd->fbi->fix.smem_start = phys;
            |-->mfd->fbi->fix.smem_len = size;

    mfd->op_enable = true;

    mutex_init(&mfd->update.lock);
    mutex_init(&mfd->no_update.lock);
    mutex_init(&mfd->mdp_sync_pt_data.sync_mutex);
    atomic_set(&mfd->mdp_sync_pt_data.commit_cnt, 0);
    atomic_set(&mfd->commits_pending, 0);

    init_timer(&mfd->no_update.timer);
    mfd->no_update.timer.function = mdss_fb_no_update_notify_timer_cb;
    mfd->no_update.timer.data = (unsigned long)mfd;
    init_completion(&mfd->update.comp);
    init_completion(&mfd->no_update.comp);
    init_completion(&mfd->power_off_comp);
    init_completion(&mfd->power_set_comp);
    init_waitqueue_head(&mfd->commit_wait_q);
    init_waitqueue_head(&mfd->idle_wait_q);

    ret = fb_alloc_cmap(&fbi->cmap, 256, 0);

    register_framebuffer(fbi);//linux 标准fbi的注册

    return 0;
}


====================================================================================================
mfd->mdp的接口函数
====================================================================================================
在mdss_fb_probe()的probe函数中
    mfd->mdp = *mdp_instance; //将mdp_instance的所有内容拷贝一份给 mfd->mdp

所以mfd->mdp等于mdp5，mdp_instance指向mdp5                                          |--------------------------------------------------------
                    |----------------------------------------------------------|    |struct mdss_overlay_private {
                    |.fb_mem_alloc_fnc = [没有定义]                            |    |   ktime_t vsync_time;
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
                    |.lut_update = [没有定义]                                  |    |   bool mixer_swap;
                    |.update_ad_input = [没有定义 ]                            |    |
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

