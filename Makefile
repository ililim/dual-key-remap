.PHONY: tests build kill rebuild restart debug release

tests:
	cl tests.c && .\tests.exe

build:
	cl .\dual-key-remap.c /link user32.lib shell32.lib /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup

kill:
	@taskkill /f /im "dual-key-remap.exe" || echo dual-key-remap is not running

rebuild:
	$(MAKE) kill
	$(MAKE) build

restart:
	$(MAKE) kill
	$(MAKE) build
	start dual-key-remap.exe

release:
	$(MAKE) kill
	$(MAKE) build
	powershell -ExecutionPolicy Bypass -File .\release.ps1
