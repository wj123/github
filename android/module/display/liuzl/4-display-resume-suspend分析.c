======================================================================================================================
1) hwcomposer initialization ,the first time open /dev/graphics/fb0   open( "/dev/graphics/fb0", O_RDWR);
======================================================================================================================
open( "/dev/graphics/fb0", O_RDWR);
|   【 fbmem.c 】
|-->fb_open(struct inode *inode, struct file *file)
    |--> info->fbops->fb_open(info,1);//mdss_fb_open(struct fb_info *info, int user)
        | 【 mdss_fb.c 】
        |-->mdss_fb_open(struct fb_info *info, int user)
            |--> mdss_fb_blank_sub(FB_BLANK_UNBLANK, info,mfd->op_enable);
                |-->mfd->mdp.on_fnc(mfd); //mdss_mdp_overlay_on()
                    |-->mdss_mdp_overlay_on(mfd)
                        |   //the first time open,initialization a ctl
                        |-->mdp5_data->ctl =  __mdss_mdp_overlay_ctl_init(mfd);//initialization ctl
                        |       |-->ctl = mdss_mdp_ctl_init(pdata, mfd);
                        |       |       ctl = mdss_mdp_ctl_alloc(mdata, MDSS_MDP_CTL0); //allocte a ctl from mdata->ctl_off
                        |       |       ctl->mfd = mfd;
                        |       |       ctl->panel_data = pdata;
                        |       |       ctl->is_video_mode = false;
                        |       |       switch (pdata->panel_info.type) {
                        |       |       case MIPI_VIDEO_PANEL:
                        |       |           ctl->is_video_mode = true;
                        |       |           ctl->intf_num = MDSS_MDP_INTF1;
                        |       |           ctl->intf_type = MDSS_INTF_DSI;
                        |       |           ctl->opmode = MDSS_MDP_CTL_OP_VIDEO_MODE;
                        |       |           ctl->start_fnc = mdss_mdp_video_start;//
                        |       |       break;
                        |       |       mdss_mdp_dither_config(&dither, NULL);
                        |       |-->ctl->vsync_handler.vsync_handler =mdss_mdp_overlay_handle_vsync;
                        |       |-->ctl->vsync_handler.cmd_post_flush = false;
                        |-->mdss_hw_init(mdss_res);
                        |-->mdss_mdp_overlay_start(mfd); //
                        |   |-->pm_runtime_get_sync(&mfd->pdev->dev);//mdss_mdp_runtime_resume()
                        |   |-->mdss_mdp_ctl_start(ctl);
                        |       |-->mdss_mdp_ctl_setup(ctl);//alocate a mixer
                        |       |-->mdss_mdp_ctl_intf_event(ctl, MDSS_EVENT_RESET, NULL);//MDSS_EVENT_RESET not used ，do nothing
                        |       |-->mdss_mdp_ctl_start_sub(ctl);
                        |            |-->ctl->start_fnc(ctl);//mdss_mdp_video_start
                        |               == mdss_mdp_video_start()
                        |                   //1、get aintF，struct mdss_mdp_video_ctx，
                        |                   //2、setup two callback，
                        |                   mdss_mdp_set_intr_callback(MDSS_MDP_IRQ_INTF_VSYNC, ctl->intf_num,
                        |                                           mdss_mdp_video_vsync_intr_done, ctl);
                        |                   mdss_mdp_set_intr_callback(MDSS_MDP_IRQ_INTF_UNDER_RUN, ctl->intf_num,
                        |                                           mdss_mdp_video_underrun_intr_done, ctl);
                        |                   //3、set intf timing-----mdss_mdp_video_timegen_setup(ctl, &itp)
                        |                   //4、register video mode other operation functions
                        |                   ctl->stop_fnc = mdss_mdp_video_stop;
                        |                   ctl->display_fnc = mdss_mdp_video_display;
                        |                   ctl->wait_fnc = mdss_mdp_video_wait4comp;
                        |                   ctl->read_line_cnt_fnc = mdss_mdp_video_line_count;
                        |                   ctl->add_vsync_handler = mdss_mdp_video_add_vsync_handler;
                        |                   ctl->remove_vsync_handler = mdss_mdp_video_remove_vsync_handler;
                        |                   ctl->config_fps_fnc = mdss_mdp_video_config_fps;
                        |
                        |-->mdss_mdp_overlay_kickoff(mfd, NULL);
================================================================================================================


================================================================================================================
2) press powerkey resume display ---- ioctl(fb_fd, FBIOBLANK,FB_BLANK_UNBLANK)
================================================================================================================
ioctl(fb_fd, FBIOBLANK,FB_BLANK_UNBLANK) -------start
|-->do_fb_ioctl()   //fbmem.c
    |-->fb_blank(info, arg);
        |-->info->fbops->fb_blank(blank, info);//mdss_fb_blank(int blank_mode, struct fb_info *info)
            |-->mdss_fb_blank_sub(blank_mode, info, mfd->op_enable);
                |-->mfd->mdp.on_fnc(mfd);// mdss_fb.c  mdss_mdp_overlay_on()
                    |-->mdss_mdp_overlay_start(mfd);
                    |   |-->pm_runtime_get_sync(&mfd->pdev->dev);//call mdp fd900000.qcom,mdss_mdp: pm_runtime [mdss_mdp_pm_resume()]
                    |   |   |-->mdss_mdp_resume_sub(mdata);
                    |   |       |-->mdss_mdp_footswitch_ctrl(mdata, true);//
                    |   |           |-->regulator_enable(mdata->fs);        // regulator enable
                    |   |           |-->mdss_mdp_cx_ctrl(mdata, true);      // enable CX power
                    |   |           |-->mdss_mdp_batfet_ctrl(mdata, true);
                    |   |
                    |   |-->mdss_hw_init(mdss_res);
                    |   |-->mdss_mdp_ctl_start(ctl, false);
                    |   |   |-->mdss_mdp_ctl_setup(ctl);
                    |   |   |-->mdss_mdp_clk_ctrl(MDP_BLOCK_POWER_ON, false); //mdp clk enable
                    |   |   |-->mdss_mdp_ctl_intf_event(ctl, MDSS_EVENT_RESET, NULL);
                    |   |   |   |--> pdata->event_handler(pdata, event, arg);//mdss_dsi_event_handler()  MDSS_EVENT_RESET  unhandled event,return
                    |   |   |-->mdss_mdp_ctl_start_sub(ctl, handoff);
                    |   |   |   |-->ctl->start_fnc(ctl);// mdss_mdp_video_start(ctl)
                    |   |   |   |   |-->mdss_mdp_mixer_get(ctl, MDSS_MDP_MIXER_MUX_LEFT); //get previously assigned mixer
                    |   |   |   |   |-->mdss_mdp_set_intr_callback(MDSS_MDP_IRQ_INTF_VSYNC, ctl->intf_num, mdss_mdp_video_vsync_intr_done, ctl);
                    |   |   |   |   |-->mdss_mdp_set_intr_callback(MDSS_MDP_IRQ_INTF_UNDER_RUN, ctl->intf_num, mdss_mdp_video_underrun_intr_done, ctl);
                    |   |   |   |       |-->mdss_mdp_video_timegen_setup(ctl, &itp)
                    |   |   |   |       |-->mdp_video_write(ctx, MDSS_MDP_REG_INTF_PANEL_FORMAT, ctl->dst_format);
                    |   |   |   |       |-->ctl->stop_fnc = mdss_mdp_video_stop;
                    |   |   |   |       |-->ctl->display_fnc = mdss_mdp_video_display;
                    |   |   |   |       |-->ctl->wait_fnc = mdss_mdp_video_wait4comp;
                    |   |   |   |       |-->ctl->read_line_cnt_fnc = mdss_mdp_video_line_count;
                    |   |   |   |       |-->ctl->add_vsync_handler = mdss_mdp_video_add_vsync_handler;
                    |   |   |   |       |-->ctl->remove_vsync_handler = mdss_mdp_video_remove_vsync_handler;
                    |   |   |   |       |-->ctl->config_fps_fnc = mdss_mdp_video_config_fps;
                    |   |   |   |-->mdss_mdp_ctl_write(ctl, MDSS_MDP_REG_CTL_LAYER(i), 0);
                    |   |   |   |-->mdss_mdp_pp_resume(ctl, mixer->num);
                    |   |   |   |-->MDSS_MDP_REG_WRITE(MDSS_MDP_REG_DISP_INTF_SEL, temp);
                    |   |   |   |-->mdp_mixer_write(mixer, MDSS_MDP_REG_LM_OUT_SIZE, outsize);
                    |   |   |-->mdss_mdp_hist_intr_setup(&mdata->hist_intr, MDSS_IRQ_RESUME);
                    |   |   |-->mdss_mdp_clk_ctrl(MDP_BLOCK_POWER_OFF, false);
                    |   |-->mdss_mdp_ctl_notifier_register(mdp5_data->ctl, &mfd->mdp_sync_pt_data.notifier);
                    |-->mdss_mdp_overlay_kickoff(mfd, NULL);
                        |-->mdss_iommu_attach(mdp5_data->mdata);//if  already dettached,skip
                        |-->mdss_mdp_pipe_queue_data(pipe, buf);
                        |-->mdss_mdp_display_commit(mdp5_data->ctl, NULL);
                            |-->mdss_mdp_mixer_update(ctl->mixer_left);
                            |-->mdss_mdp_ctl_write(ctl, MDSS_MDP_REG_CTL_TOP, ctl->opmode);
                            |-->mdss_mdp_ctl_notify(ctl, MDP_NOTIFY_FRAME_READY);
                            |-->mdss_mdp_ctl_write(ctl, MDSS_MDP_REG_CTL_FLUSH, ctl->flush_bits);
                            |--> ctl->display_fnc(ctl, arg); /* kickoff */  //mdss_mdp_video_display
                                |-->video_vsync_irq_enable(ctl, true);   // mdss_mdp_irq_enable: MDP IRQ mask old=0 new=8000000
                                |   //if (!ctx->timegen_en) {
                                |-->mdss_mdp_ctl_intf_event(ctl, MDSS_EVENT_UNBLANK, NULL);
                                    |--> mdss_dsi_event_handler(ctl, MDSS_EVENT_UNBLANK, NULL);
                                        |-->mdss_dsi_on(pdata);        //lcd_power_on
                                        |-->mdss_dsi_op_mode_config(pdata->panel_info.mipi.mode,pdata);
                                        |-->mdss_dsi_unblank(pdata); //if (ctrl_pdata->on_cmds.link_state == DSI_LP_MODE)
                                        |   |-->ctrl_pdata->on(pdata);  //mdss_dsi_panel_on()
                                        |       
                                        |       
                                        |       
                                        |       
                                        |       
                                        |       
                                        |       
                                        |       
                                        |       
                                        |       
                                        |       
                                        |       
                                        |       
                                        |
                                        |-->mdss_mdp_clk_ctrl(MDP_BLOCK_POWER_ON, false);
                                        |-->mdss_mdp_irq_enable(MDSS_MDP_IRQ_INTF_UNDER_RUN, ctl->intf_num);
                                        |-->mdp_video_write(ctx, MDSS_MDP_REG_INTF_TIMING_ENGINE_EN, 1); //"Host Display Data transfer"
                                        |-->mdss_mdp_ctl_intf_event(ctl, MDSS_EVENT_PANEL_ON, NULL);
                                            |-->mdss_dsi_event_handler(ctl, MDSS_EVENT_PANEL_ON, NULL);
                                                |-->ctrl_pdata->ctrl_state |= CTRL_STATE_MDP_ACTIVE;
                                                |-->if (ctrl_pdata->on_cmds.link_state == DSI_HS_MODE)    //now code skip
                                                |-->    rc = mdss_dsi_unblank(pdata);
---------------------ioctl(fb_fd, FBIOBLANK,FB_BLANK_UNBLANK) -------end    wait lcd-backlight on

================================================================================================================
3) enable vsync ---- ioctl(fb_fd, MSMFB_OVERLAY_VSYNC_CTRL,&enable)
================================================================================================================
ioctl(fb_fd, MSMFB_OVERLAY_VSYNC_CTRL,enable)
|-->mdss_fb_ioctl(struct fb_info *info, unsigned int cmd,unsigned long arg)
    |--> mfd->mdp.ioctl_handler(mfd, cmd, argp); //  mdss_mdp_overlay_ioctl_handler()
        |-->mdss_mdp_overlay_vsync_ctrl(mfd, val);
            |-->ctl->add_vsync_handler(ctl, &ctl->vsync_handler);//mdss_mdp_video_add_vsync_handler()
                |--> mdss_mdp_video_add_vsync_handler()
                        -->mdss_mdp_irq_enable()-->mdss_enable_irq() // enable vsync irq

================================================================================================================
4) commit ---- ioctl(fd, MSMFB_DISPLAY_COMMIT, &info)
================================================================================================================
ioctl(fd, MSMFB_DISPLAY_COMMIT, &info)
    |-->mdss_fb_ioctl:MSMFB_DISPLAY_COMMIT
        |-->mdss_fb_display_commit(info, argp);
            |--> mdss_fb_pan_display_ex(info, &disp_commit);

================================================================================================================
5) set lcd-backlight
================================================================================================================
brightness_set = mdss_fb_set_bl_brightness,
mdss_fb_set_bl_brightness(struct led_classdev *led_cdev,enum led_brightness value)
    |-->mdss_fb_set_backlight(mfd, bl_lvl);
        |-->pdata->set_backlight(pdata, temp);    //panel_data.set_backlight = mdss_dsi_panel_bl_ctrl;
            |-->mdss_dsi_panel_bl_ctrl()
                |-->                                                                 
                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                     
                                                                                     
                                                                                     
================================================================================================================
6) allocte pipe ---- ioctl(fd, MSMFB_OVERLAY_SET, &ov)
================================================================================================================
ioctl(fd, MSMFB_OVERLAY_SET, &ov)
    |-->mdss_mdp_overlay_ioctl_handler:MSMFB_OVERLAY_SET
        |--> mdss_mdp_overlay_set(mfd, req);
            |-->mdss_mdp_rotator_setup(mfd, req);  //if (req->flags & MDSS_MDP_ROT_ONLY)
            |-->req->id = BORDERFILL_NDX; // if (req->src.format == MDP_RGB_BORDERFILL)
            |-->mdss_mdp_overlay_pipe_setup(mfd, req, &pipe);
                |-->mdss_mdp_overlay_pipe_setup(mfd, req, &pipe);

================================================================================================================
7) overlay play ---- ioctl(fd, MSMFB_OVERLAY_PLAY, &od)
================================================================================================================
ioctl(fd, MSMFB_OVERLAY_PLAY, &od)
|-->mdss_mdp_overlay_ioctl_handler:MSMFB_OVERLAY_PLAY
    |--> mdss_mdp_overlay_play(mfd, &data);
        |-->mdss_mdp_overlay_start(mfd);
        |--> mdss_mdp_overlay_queue(mfd, req);
            |-->pipe = mdss_mdp_pipe_get(mdata, req->id);
            |-->mdss_mdp_overlay_get_buf(mfd, src_data, &req->data, 1, flags);
            |   |-->mdss_mdp_get_img(&planes[i], &data->p[i]);
            |-->mdss_mdp_pipe_unmap(pipe);










