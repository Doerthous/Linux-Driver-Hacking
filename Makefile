SUBDIRS= \
	001_module \
	002_sysfs \
	003_platform_driver \
	004_gpio_legacy \
	005_timer \
	006_kthread \
	006_workqueue \
	007_iio_device \
	008_input_device \
	platform/rpi4b

clean:
	@for subdir in $(SUBDIRS); do make -C $$subdir clean; done