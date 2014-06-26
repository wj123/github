========================================================================================
Mdss_dsi.c (kernel\drivers\video\msm\mdss) /sys/devices/fd922800.qcom,mdss_dsi
root@SSS999ZZ:/sys/devices/fd922800.qcom,mdss_dsi # ls -l
lrwxrwxrwx root     root              1970-01-01 00:14 driver -> ../../bus/platform/drivers/mdss_dsi_ctrl
-r--r--r-- root     root         4096 1970-01-01 00:14 microamps_requested_fd922800.qcom,mdss_dsi-vdd
-r--r--r-- root     root         4096 1970-01-01 00:14 microamps_requested_fd922800.qcom,mdss_dsi-vdda
-r--r--r-- root     root         4096 1970-01-01 00:14 microamps_requested_fd922800.qcom,mdss_dsi-vddio
-r--r--r-- root     root         4096 1970-01-01 00:14 modalias
drwxr-xr-x root     root              1970-01-01 00:14 power
lrwxrwxrwx root     root              1970-01-01 00:14 subsystem -> ../../bus/platform
-rw-r--r-- root     root         4096 1970-01-01 00:14 uevent
========================================================================================
static const struct of_device_id mdss_dsi_ctrl_dt_match[] = {
    {.compatible = "qcom,mdss-dsi-ctrl"},
    {}
};
MODULE_DEVICE_TABLE(of, mdss_dsi_ctrl_dt_match);

static struct platform_driver mdss_dsi_ctrl_driver = {
    .probe = mdss_dsi_ctrl_probe,
    .remove = __devexit_p(mdss_dsi_ctrl_remove),
    .shutdown = NULL,
    .driver = {
        .name = "mdss_dsi_ctrl",
        .of_match_table = mdss_dsi_ctrl_dt_match,
    },
};
======================================================================================== 
mdss_dsi_ctrl_probe函数的主要功能：
	1.1、为关键驱动数据结构 struct mdss_dsi_ctrl_pdata *ctrl_pdata 分配空间
	1.2、获得 struct resource 资源	//【IORESOURCE_MEM】【IORESOURCE_IRQ】
	1.3、解释dts 的platform-supply-entry为 struct mdss_data_type *mdata 配置power_data	//mdss_dsi_get_dt_vreg_data()
	1.4、获得panel的dts node;
	1.5、解释panel的dts，为ctrl_pdata->panel_data.panel_info赋值，并挂接如下三个重要的函数
			    ctrl_pdata->on = mdss_dsi_panel_on;			//函数指针
		        ctrl_pdata->off = mdss_dsi_panel_off;		//函数指针
		        ctrl_pdata->panel_data.set_backlight = mdss_dsi_panel_bl_ctrl;	//函数指针
	1.6、调用dsi_panel_device_register(dsi_pan_node, ctrl_pdata); 
		1.6.1、dsi clk的分频设置、clk计算 //mdss_dsi_clk_div_config(pinfo, mipi->frame_rate);
		1.6.2、regulator_init //mdss_dsi_regulator_init()
		1.6.3、dsi phy configuration  //
		1.6.4、LCD gpio get and configuration
		1.6.5、dsi clk get //mdss_dsi_clk_init()
		1.6.6、ctrl_pdata->panel_data.event_handler = mdss_dsi_event_handler;  // 关键函数 
		1.6.7、ctrl_pdata->check_status = mdss_dsi_bta_status_check;			//	
		1.6.8、注册dsi中断      //mdss_register_irq(ctrl->dsi_hw)
		1.6.9、mdss_register_panel(ctrl_pdev, &(ctrl_pdata->panel_data));   //注册qcom,mdss_fb_primary设备
		1.6.0、dsi debug node 创建 
----------------------------------------------------------------------------------------  
1、mdss_dsi_ctrl驱动的probe函数分析（删除了一些无关紧要的代码）：
----------------------------------------------------------------------------------------
static int __devinit mdss_dsi_ctrl_probe(struct platform_device *pdev)
{
    struct mdss_dsi_ctrl_pdata *ctrl_pdata = NULL;
    ctrl_pdata = devm_kzalloc(&pdev->dev,sizeof(struct mdss_dsi_ctrl_pdata),GFP_KERNEL); //分配关键结构体
    platform_set_drvdata(pdev, ctrl_pdata);
    ctrl_name = of_get_property(pdev->dev.of_node, "label", NULL);//label = "MDSS DSI CTRL->0";
    of_property_read_u32(pdev->dev.of_node,"cell-index", &index);//cell-index = <0>;
    pdev->id = 1;
    mdss_dsi_mres = platform_get_resource(pdev, IORESOURCE_MEM, 0);//reg = <0xfd922800 0x600>;
    mdss_dsi_base = ioremap(mdss_dsi_mres->start, resource_size(mdss_dsi_mres)); //映射
    of_platform_populate(pdev->dev.of_node, NULL, NULL, &pdev->dev);
    mdss_dsi_get_dt_vreg_data(&pdev->dev, &ctrl_pdata->power_data);//配置power_data
    	---- start ------
			qcom,platform-supply-entry1 {
				qcom,supply-name = "vdd";
				qcom,supply-min-voltage = <0x2ab980>;
				qcom,supply-max-voltage = <0x2ab980>;
				qcom,supply-enable-load = <0x186a0>;
				qcom,supply-disable-load = <0x64>;
				qcom,supply-pre-on-sleep = <0x0>;
				qcom,supply-post-on-sleep = <0x0>;
				qcom,supply-pre-off-sleep = <0x0>;
				qcom,supply-post-off-sleep = <0x0>;
			};

			qcom,platform-supply-entry2 {
				qcom,supply-name = "vddio";
				qcom,supply-min-voltage = <0x1b7740>;
				qcom,supply-max-voltage = <0x1b7740>;
				qcom,supply-enable-load = <0x186a0>;
				qcom,supply-disable-load = <0x64>;
				qcom,supply-pre-on-sleep = <0x0>;
				qcom,supply-post-on-sleep = <0x0>;
				qcom,supply-pre-off-sleep = <0x0>;
				qcom,supply-post-off-sleep = <0x0>;
			};

			qcom,platform-supply-entry3 {										
				qcom,supply-name = "vdda";                      				| mp->vreg_config[i].vreg_name,    
				qcom,supply-min-voltage = <0x124f80>;           				| mp->vreg_config[i].min_voltage,  
				qcom,supply-max-voltage = <0x124f80>;           				| mp->vreg_config[i].max_voltage,  
				qcom,supply-enable-load = <0x186a0>;            				| mp->vreg_config[i].enable_load,  
				qcom,supply-disable-load = <0x64>;              				| mp->vreg_config[i].disable_load, 
				qcom,supply-pre-on-sleep = <0x0>;               				| mp->vreg_config[i].pre_on_sleep, 
				qcom,supply-post-on-sleep = <0x14>;             				| mp->vreg_config[i].post_on_sleep,
				qcom,supply-pre-off-sleep = <0x0>;              				| mp->vreg_config[i].pre_off_sleep,
				qcom,supply-post-off-sleep = <0x0>;                             | mp->vreg_config[i].post_off_sleep
			};    	  	
    	----- end -----
    mdss_dsi_get_panel_cfg(panel_cfg);//mdss_mdp_probe()中的mdss_mdp_parse_bootarg()函数中初始化的。 // mdss_res->pan_cfg
    //<3>[    0.818084] mdss_dsi_find_panel_of_node: invalid pan node, selecting prim panel
    dsi_pan_node = mdss_dsi_find_panel_of_node(pdev, panel_cfg);
    	----- start ------
       	mdss_dsi_pref_prim_panel()
       	|-->dsi_pan_node = of_parse_phandle(pdev->dev.of_node,"qcom,dsi-pref-prim-pan", 0);
    	【 XXXXXXXXXX-shdisp.dtsi (kernel\arch\arm\boot\dts) 】
		    &mdss_dsi0 {
			qcom,dsi-pref-prim-pan=<&dsi_shdisp_carin>;		
			/delete-property/ qcom,platform-reset-gpio;
			/delete-property/ qcom,platform-enable-gpio;		
			};

   		 ----- end ------
    cmd_cfg_cont_splash = mdss_panel_get_boot_cfg() ? true : false; //cmd_cfg_cont_splash = false
    mdss_dsi_panel_init(dsi_pan_node, ctrl_pdata, cmd_cfg_cont_splash);
    	@@@@@@@@@ mdss_dsi_panel_init()  分析 start  @@@@@@@@@
        panel_name = of_get_property(node, "qcom,mdss-dsi-panel-name", NULL);// mdss_dsi_panel_init: Panel Name =XXXXXXXXX video mode dsi panel
        mdss_panel_parse_dt(node, ctrl_pdata);
            struct mdss_panel_info *pinfo = &(ctrl_pdata->panel_data.panel_info);
                    解释如下项：
            ----------start---------    
            qcom,mdss-dsi-panel-width = <720>;
            qcom,mdss-dsi-panel-height = <1280>;
			qcom,mdss-pan-physical-width-dimension = <0x3e>; //62
			qcom,mdss-pan-physical-height-dimension = <0x6e>; //110

            qcom,mdss-dsi-h-left-border = <0>;
            qcom,mdss-dsi-h-right-border = <0>;
            qcom,mdss-dsi-v-top-border = <0>;
            qcom,mdss-dsi-v-bottom-border = <0>;
            qcom,mdss-dsi-bpp = <24>;
            qcom,mdss-dsi-panel-type = "dsi_video_mode";
            qcom,mdss-dsi-pixel-packing = //not fount
            qcom,mdss-dsi-panel-destination = "display_1";
			qcom,mdss-dsi-h-front-porch = <112>;
			qcom,mdss-dsi-h-back-porch = <44>;
			qcom,mdss-dsi-h-pulse-width = <16>;
			qcom,mdss-dsi-h-sync-skew = <0>;
			qcom,mdss-dsi-v-back-porch = <10>;
			qcom,mdss-dsi-v-front-porch = <10>;
			qcom,mdss-dsi-v-pulse-width = <2>;
            qcom,mdss-dsi-color-order = <0>;
            qcom,mdss-dsi-underflow-color = <0xff>;
            qcom,mdss-dsi-bl-pmic-control-type = "none";
			qcom,mdss-dsi-bl-min-level = <0x0>;
			qcom,mdss-dsi-bl-max-level = <0x16>;//22

            qcom,mdss-dsi-interleave-mode = //not fount
            qcom,mdss-dsi-te-check-enable;
			qcom,mdss-dsi-te-using-te-pin;
            qcom,mdss-dsi-h-sync-pulse = <0>;      
            qcom,mdss-dsi-bllp-eof-power-mode;
            qcom,mdss-dsi-bllp-power-mode;                  
            qcom,mdss-dsi-hfp-power-mode = //not fount
            qcom,mdss-dsi-hsa-power-mode = //not fount
            qcom,mdss-dsi-hbp-power-mode = //not fount
            qcom,mdss-dsi-traffic-mode = <0x1>;
            qcom,mdss-dsi-te-dcs-command = //not fount
            qcom,mdss-dsi-te-v-sync-continue-lines = //not fount
            qcom,mdss-dsi-te-v-sync-rd-ptr-irq-line = //not fount
            qcom,mdss-dsi-te-pin-select = <0x1>;
            qcom,mdss-dsi-virtual-channel-id = <0>;
            qcom,mdss-dsi-color-order = <0>;
            qcom,mdss-dsi-lane-map = <0>;
			qcom,mdss-dsi-t-clk-post = <0x4>;
			qcom,mdss-dsi-t-clk-pre = <0x1a>;
            qcom,mdss-dsi-stream = <0>;
            qcom,mdss-dsi-dma-trigger = <4>;
            qcom,mdss-dsi-mdp-trigger = <0>;
            qcom,mdss-dsi-panel-mode-gpio-state = //not fount
            qcom,mdss-dsi-panel-framerate = <60>;
			qcom,mdss-dsi-panel-clockrate = <418500000>;
            qcom,mdss-dsi-panel-timings = [7b 1a 10 00 3e 40 16 1e 15 03 04 00];
            qcom,mdss-dsi-lp11-init = //not fount
            qcom,mdss-dsi-reset-sequence = //not fount
			qcom,mdss-dsi-on-command-state = "dsi_lp_mode";
			qcom,mdss-dsi-off-command-state = "dsi_lp_mode";
            ----------end---------
            解释后的赋值：
            ------ start -----
            pinfo->xres =720;       //qcom,mdss-dsi-panel-width = <720>;
            pinfo->yres =1280;      //qcom,mdss-dsi-panel-height = <1280>;
            pinfo->physical_width = 62;     //qcom,mdss-pan-physical-width-dimension = <62>;
            pinfo->physical_height = 110;   //qcom,mdss-pan-physical-height-dimension = <110>;
            pinfo->lcdc.xres_pad = 0;		//qcom,mdss-dsi-h-left-border + qcom,mdss-dsi-h-right-border
            pinfo->lcdc.yres_pad = 0;		//qcom,mdss-dsi-v-top-border + qcom,mdss-dsi-v-bottom-border
            pinfo->bpp = 24;		//qcom,mdss-dsi-bpp = <24>;
            pinfo->mipi.mode = DSI_VIDEO_MODE;	//qcom,mdss-dsi-panel-type = "dsi_video_mode";
            //qcom,mdss-dsi-pixel-packing not fount, so tmp=0
            ////mdss_panel_dt_get_dst_fmt(pinfo->bpp,pinfo->mipi.mode, tmp,&(pinfo->mipi.dst_format));
            pinfo->mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888；
            pinfo->pdest = DISPLAY_1； //qcom,mdss-dsi-panel-destination = "display_1";
            pinfo->lcdc.h_front_porch = 112;	//qcom,mdss-dsi-h-front-porch = <112>;
            pinfo->lcdc.h_back_porch = 44;		//qcom,mdss-dsi-h-back-porch = <44>;
            pinfo->lcdc.h_pulse_width = 16;		//qcom,mdss-dsi-h-pulse-width = <16>;		
            pinfo->lcdc.hsync_skew = 0;			//qcom,mdss-dsi-h-sync-skew = <0>;
            pinfo->lcdc.v_back_porch = 10;		//qcom,mdss-dsi-v-back-porch = <10>;
            pinfo->lcdc.v_front_porch = 10;		//qcom,mdss-dsi-v-front-porch = <10>;
            pinfo->lcdc.v_pulse_width = 2;		//qcom,mdss-dsi-v-pulse-width = <2>;
            pinfo->lcdc.underflow_clr = 0xff;//blue		//qcom,mdss-dsi-underflow-color = <0xff>;
            pinfo->lcdc.border_clr = 0;//black 黑色	  //qcom,mdss-dsi-border-color = <0>;
            pinfo->bklt_ctrl = UNKNOWN_CTRL;		//qcom,mdss-dsi-bl-pmic-control-type = "none";
            pinfo->bl_min = 0;						//qcom,mdss-dsi-bl-min-level = <0>;
            pinfo->bl_max = 22;						//qcom,mdss-dsi-bl-max-level = <22>;
            ctrl_pdata->bklt_max = pinfo->bl_max = 22;
            pinfo->mipi.interleave_mode = false;  	//qcom,mdss-dsi-interleave-mode = //not fount       
            pinfo->mipi.vsync_enable = true;		//qcom,mdss-dsi-te-check-enable;
            pinfo->mipi.hw_vsync_mode = true;		//qcom,mdss-dsi-te-using-te-pin;
            pinfo->mipi.pulse_mode_hsa_he = 0;      //qcom,mdss-dsi-h-sync-pulse = <0>;  
            pinfo->mipi.hfp_power_stop = false; 	//qcom,mdss-dsi-hfp-power-mode = //not fount          
            pinfo->mipi.hsa_power_stop = false; 	//qcom,mdss-dsi-hsa-power-mode = //not fount
            pinfo->mipi.hbp_power_stop = false; 	//qcom,mdss-dsi-hbp-power-mode = //not fount
            pinfo->mipi.bllp_power_stop = true;		//qcom,mdss-dsi-bllp-power-mode;   
            pinfo->mipi.eof_bllp_power_stop = true;	//qcom,mdss-dsi-bllp-eof-power-mode;
            pinfo->mipi.traffic_mode = 1;    DSI_NON_BURST_SYNCH_EVENT	//qcom,mdss-dsi-traffic-mode = <0x01>
            pinfo->mipi.insert_dcs_cmd = false; 				//qcom,mdss-dsi-te-dcs-command = //not fount
            pinfo->mipi.wr_mem_continue =(!rc ? tmp : 0x3c); 	//qcom,mdss-dsi-te-v-sync-continue-lines 没有定义
            pinfo->mipi.wr_mem_start =(!rc ? tmp : 0x2c); 		//qcom,mdss-dsi-te-v-sync-rd-ptr-irq-line 没有定义
            pinfo->mipi.te_sel = (!rc ? tmp : 1); 				//qcom,mdss-dsi-te-pin-select = <0x1>;
            pinfo->mipi.vc = 0;									//qcom,mdss-dsi-virtual-channel-id = <0>
            pinfo->mipi.rgb_swap = 0; //DSI_RGB_SWAP_RGB		//qcom,mdss-dsi-color-order = <0>;
            pinfo->mipi.data_lane0 = 1;		//qcom,mdss-dsi-lane-0-state;
            pinfo->mipi.data_lane1 = 1;		//qcom,mdss-dsi-lane-1-state;
            pinfo->mipi.data_lane2 = 1;		//qcom,mdss-dsi-lane-2-state;
            pinfo->mipi.data_lane3 = 1;		//qcom,mdss-dsi-lane-3-state;
            pinfo->mipi.dlane_swap = 0;     //qcom,mdss-dsi-lane-map = <0>;		
            pinfo->mipi.t_clk_pre = 0x4;	//qcom,mdss-dsi-t-clk-pre = <0x1a>;
            pinfo->mipi.t_clk_post = 0x1a;	//qcom,mdss-dsi-t-clk-post = <0x4>;
            pinfo->mipi.stream = 0; 		//qcom,mdss-dsi-stream = <0>;
            pinfo->mipi.mdp_trigger = 0; // DSI_CMD_TRIGGER_NONE	//qcom,mdss-dsi-mdp-trigger = <0x0>;
            pinfo->mipi.dma_trigger = 4; // DSI_CMD_TRIGGER_SW		//qcom,mdss-dsi-dma-trigger = <0x4>;
            pinfo->mode_gpio_state = MODE_GPIO_NOT_VALID; //qcom,mdss-dsi-panel-mode-gpio-state = //not fount
            pinfo->mipi.frame_rate = (!rc ? tmp : 60); //qcom,mdss-dsi-panel-framerate = <60>;
            pinfo->clk_rate = 0； //qcom,mdss-dsi-panel-clockrate = <418500000>;
            pinfo->mipi.dsi_phy_db.timing[ ] =  [7b 1a 10 00 3e 40 16 1e 15 03 04 00];//qcom,mdss-dsi-panel-timings = [7b 1a 10 00 3e 40 16 1e 15 03 04 00];
            pinfo->mipi.lp11_init = false //qcom,mdss-dsi-lp11-init 没有定义
            pinfo->mipi.init_delay = (!rc ? tmp : 0);//qcom,mdss-dsi-init-delay-us 没有定义
            //qcom,mdss-dsi-fbc-enable not fount 没有定义
            mdss_dsi_parse_fbc_params(np, pinfo);
                panel_info->fbc.enabled = 0;
                panel_info->fbc.target_bpp = panel_info->bpp;
            //qcom,mdss-dsi-reset-sequence = not fount;
            mdss_dsi_parse_reset_seq(np, pinfo->rst_seq, &(pinfo->rst_seq_len),
                "qcom,mdss-dsi-reset-sequence");
            //qcom,mdss-dsi-on-command = [05 01 00 00 00 00 01 00]; <0x5010000 0x100>;
            mdss_dsi_parse_dcs_cmds(np, &ctrl_pdata->on_cmds,
                "qcom,mdss-dsi-on-command", "qcom,mdss-dsi-on-command-state");
            //qcom,mdss-dsi-off-command = [05 01 00 00 00 00 01 00]; <0x5010000 0x100>;
            mdss_dsi_parse_dcs_cmds(np, &ctrl_pdata->off_cmds,
                "qcom,mdss-dsi-off-command", "qcom,mdss-dsi-off-command-state");
           ------ end -----
        ctrl_pdata->panel_data.panel_info.cont_splash_enabled = 0;
        //qcom,partial-update-enabled  not fount 没有定义
        ctrl_pdata->panel_data.panel_info.partial_update_enabled = 0;
        ctrl_pdata->partial_update_fnc = NULL;
        ctrl_pdata->on = mdss_dsi_panel_on;			//函数指针
        ctrl_pdata->off = mdss_dsi_panel_off;		//函数指针
        ctrl_pdata->panel_data.set_backlight = mdss_dsi_panel_bl_ctrl;	//函数指针
        @@@@@@@@@ mdss_dsi_panel_init()  分析 end  @@@@@@@@@
    dsi_panel_device_register(dsi_pan_node, ctrl_pdata); //下面分析
}

==========================================================================================
2、dsi_panel_device_register函数分析
==========================================================================================
dsi_panel_device_register(dsi_pan_node, ctrl_pdata); 
{
	struct mdss_panel_info *pinfo = &(ctrl_pdata->panel_data.panel_info);                                                                         
	mipi  = &(pinfo->mipi); 
	pinfo->type = MIPI_VIDEO_PANEL；
	mdss_dsi_clk_div_config(pinfo, mipi->frame_rate); //clk分频设置
	pinfo->mipi.dsi_pclk_rate = dsi_pclk_rate;
	dsi_ctrl_np = of_parse_phandle(pan_node,"qcom,mdss-dsi-panel-controller", 0); //qcom,mdss-dsi-panel-controller = <&mdss_dsi0>;    
	ctrl_pdev = of_find_device_by_node(dsi_ctrl_np);//get node【 mdss_dsi0: qcom,mdss_dsi@fd922800  】
	mdss_dsi_regulator_init(ctrl_pdev);
		 msm_dss_config_vreg(&pdev->dev, ctrl_pdata->power_data.vreg_config, ctrl_pdata->power_data.num_vreg, 1)；
	@@@@@@@@@@ dsi phy configuration start @@@@@@@@@@@   
	|-----------------------------|
	|/* DSI PHY configuration */  |
	|struct mdss_dsi_phy_ctrl {   |
	|	uint32_t regulator[7];    |
	|	uint32_t timing[12];      |
	|	uint32_t ctrl[4];         |
	|	uint32_t strength[2];     |
	|	char bistctrl[6];         |
	|	uint32_t pll[21];         |
	|	char lanecfg[45];         |
	|};                           |
	|-----------------------------|
	data = of_get_property(ctrl_pdev->dev.of_node,"qcom,platform-strength-ctrl", &len);// qcom,platform-strength-ctrl = [ff 06];
	pinfo->mipi.dsi_phy_db.strength[0] = data[0]; //ff
	pinfo->mipi.dsi_phy_db.strength[1] = data[1]; //06
	//qcom,platform-regulator-settings = [07 09 03 00 20 00 01];
	data = of_get_property(ctrl_pdev->dev.of_node,"qcom,platform-regulator-settings", &len);
	pinfo->mipi.dsi_phy_db.regulator[ ] = data[ ];	//[07 09 03 00 20 00 01];
	data = of_get_property(ctrl_pdev->dev.of_node,"qcom,platform-bist-ctrl", &len);/qcom,platform-bist-ctrl = [00 00 b1 ff 00 00];
	pinfo->mipi.dsi_phy_db.bistctrl[ ] = data[ ]; //[00 00 b1 ff 00 00]
	---------- start ----------
	qcom,platform-lane-config = 
	       [00 00 00 00 00 00 00 01 97
			00 00 00 00 05 00 00 01 97
			00 00 00 00 0a 00 00 01 97
			00 00 00 00 0f 00 00 01 97
			00 c0 00 00 00 00 00 01 bb];
	----------- end -----------
	data = of_get_property(ctrl_pdev->dev.of_node,"qcom,platform-lane-config", &len);
	pinfo->mipi.dsi_phy_db.lanecfg[i] = data[i];  
	@@@@@@@@@@ dsi phy configuration end @@@@@@@@@@@  
	//qcom,mdss-dsi-panel-broadcast-mode 没有定义  ctrl_pdata->shared_pdata.broadcast_enable = false
	ctrl_pdata->shared_pdata.broadcast_enable = of_property_read_bool(pan_node, "qcom,mdss-dsi-panel-broadcast-mode");
	//qcom,mdss-dsi-pan-enable-dynamic-fps没有定义 dynamic_fps = false   
	dynamic_fps = of_property_read_bool(pan_node,"qcom,mdss-dsi-pan-enable-dynamic-fps");// 没有定义 
	//qcom,platform-enable-gpio 没有定义
	ctrl_pdata->disp_en_gpio = of_get_named_gpio(ctrl_pdev->dev.of_node,"qcom,platform-enable-gpio", 0); // 没有定义 
	//qcom,platform-reset-gpio = <&msmgpio 25 0>;  msm8226-mdss.dtsi 文件中
	//qcom,platform-te-gpio = <&msmgpio 24 0>;
	//	/delete-property/ qcom,platform-reset-gpio;  XXXXXXXXXXXX-shdisp.dtsi  文件中
	//	/delete-property/ qcom,platform-enable-gpio;	
	mdss_dsi_clk_init(ctrl_pdev, ctrl_pdata)//clk clk_get
		|--> ctrl_pdata->mdp_core_clk = clk_get(dev, "mdp_core_clk");
		|--> ctrl_pdata->ahb_clk = clk_get(dev, "iface_clk");
		|--> ctrl_pdata->axi_clk = clk_get(dev, "bus_clk");
		|--> ctrl_pdata->byte_clk = clk_get(dev, "byte_clk");
		|--> ctrl_pdata->pixel_clk = clk_get(dev, "pixel_clk");
		|--> ctrl_pdata->esc_clk = clk_get(dev, "core_clk");
	mdss_dsi_retrieve_ctrl_resources(ctrl_pdev, pinfo->pdest, ctrl_pdata)
		|--> rc = of_property_read_u32(pdev->dev.of_node, "cell-index", &index);
		|--> mdss_dsi_mres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		|--> ctrl->ctrl_base = ioremap(mdss_dsi_mres->start, resource_size(mdss_dsi_mres));
		|--> ctrl->reg_size = resource_size(mdss_dsi_mres);
	ctrl_pdata->panel_data.event_handler = mdss_dsi_event_handler;  // 关键函数 
	ctrl_pdata->check_status = mdss_dsi_bta_status_check;			//	bta check			
	//if (ctrl_pdata->bklt_ctrl == BL_PWM)  //because  ctrl_pdata->bklt_ctrl = UNKNOWN_CTRL
	//	mdss_dsi_panel_pwm_cfg(ctrl_pdata);
	mdss_dsi_ctrl_init(ctrl_pdata);
		|-->ctrl_pdata结构体中一些成员的初始化
		|-->mdss_register_irq(ctrl->dsi_hw) //注册dsi中断
			|---------------------------------|mdss_register_irq（&mdss_mdp_hw）函数的功能是：
			| struct mdss_hw mdss_dsi0_hw = { |mdss_irq_handlers[hw->hw_ndx] = mdss_dsi0_hw;
			| 	.hw_ndx = MDSS_HW_DSI0,       |
			| 	.ptr = mdss_dsi_ctrl_pdata *ctrl|         
			| 	.irq_handler = mdss_dsi_isr,  |		
			| };                              |  
			|---------------------------------|   
	ctrl_pdata->pclk_rate = mipi->dsi_pclk_rate;
	ctrl_pdata->byte_clk_rate = pinfo->clk_rate / 8;
	ctrl_pdata->ctrl_state = CTRL_STATE_UNKNOWN;
	pinfo->panel_power_on = 0;
	mdss_register_panel(ctrl_pdev, &(ctrl_pdata->panel_data));
	if (pinfo->pdest == DISPLAY_1) {
		mdss_debug_register_base("dsi0", ctrl_pdata->ctrl_base, ctrl_pdata->reg_size); //debugfs
		ctrl_pdata->ndx = 0;
} 
============================================================================================
3、qcom,mdss_fb_primary 设备注册 mdss_register_panel()
        |-----------------------------------------------------|
		|mdss_fb0: qcom,mdss_fb_primary {                     |
		|	cell-index = <0>;                                 |
		|	compatible = "qcom,mdss-fb";                      |
		|	qcom,memory-reservation-type = "EBI1";            |
		|	qcom,memory-reservation-size = <0x800000>;        |
		|	qcom,memblock-reserve = <0x03200000 0xFA0000>;    |
        |};                                                   |
        |-----------------------------------------------------|  
============================================================================================ 
mdss_register_panel(ctrl_pdev, &(ctrl_pdata->panel_data));    
{	         
	struct platform_device *fb_pdev, *mdss_pdev;
	//在mdss_dsi0: qcom,mdss_dsi@fd922800 node中有 qcom,mdss-fb-map = <&mdss_fb0>;                                
	node = of_parse_phandle(pdev->dev.of_node, "qcom,mdss-fb-map", 0);
	mdss_pdev = of_find_device_by_node(node->parent);//得到 mdss_mdp: qcom,mdss_mdp@fd900000 node  的 platform_device
	fb_pdev = of_find_device_by_node(node);//没有发现这个qcom,mdss_fb_primary的设备platform_device，因为还没有注册
	if (fb_pdev) {
		rc = mdss_fb_register_extra_panel(fb_pdev, pdata); //如果是双屏，这个是第二块屏使用
	} else {
		pr_info("adding framebuffer device %s\n", dev_name(&pdev->dev));
		///注册qcom,mdss_fb_primary设备，设备注册完成后将与驱动的match,match后调用 mdss_fb_probe(),这个将在XXXXXXXXXX-fb分析code.c中分析
		fb_pdev = of_platform_device_create(node, NULL,&mdss_pdev->dev);
		fb_pdev->dev.platform_data = pdata;  // &(ctrl_pdata->panel_data)
	}

	if (mdp_instance->panel_register_done)
		mdp_instance->panel_register_done(pdata);//mdss_panel_register_done() 
}							|
							V
int mdss_panel_register_done(struct mdss_panel_data *pdata)
{
	if (pdata->panel_info.cont_splash_enabled) { //pdata->panel_info.cont_splash_enabled这个是0，所以这个函数什么也没做
		mdss_mdp_clk_ctrl(MDP_BLOCK_POWER_ON, false);
		mdss_mdp_footswitch_ctrl_splash(1);
	}
	return 0;
}
