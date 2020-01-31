.PHONY: install-deps
install-deps:
	sudo apt install python3-pip ninja libmbedcrypto3 libmbedtls-dev -y
	pip3 install meson

