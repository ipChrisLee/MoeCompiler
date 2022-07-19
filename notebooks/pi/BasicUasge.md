# Configurations

## Rsapi-Config

* hostname : `raspberrypi.local`
* username : pi
* connected with WLAN
* IP Address : `192.168.0.101` (May be changed.)

# Connection

## Make Pi automatically connect to Wi-Fi and open SSH

Put micro SD card into computer, create `/boot/wpa_supplicant.conf` and write the
following:

```text
country=CN
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
	ssid="wifi_name"
	psk="password"
}
```

To enable ssh, create `/boot/ssh.txt` which will enable ssh connection.

Use `arp -a` or log in Wi-Fi manager to get IP address of raspberry.

## Authorize Key from Computer

[Ref Site](https://www.digitalocean.com/community/tutorials/how-to-configure-ssh-key-based-authentication-on-a-linux-server)

In one word: `ssh-copy-id pi@192.168.0.101`.

Then log in to Pi, and select what keys to be remained in `.ssh/authorized_keys`.

## Connect Pi on computer with ssh

`ssh pi@192.168.0.101 -p 22` (change `192.168.0.101` to ip)

`ssh` can also run command from
computer: `ssh pi@192.168.0.101 "sudo shutdown -h now"`.

## File Transfer between Pi and Computer

From computer to Pi:
`scp test.c pi@192.168.0.101:~`

Get file from pi to computer:
`scp pi@192.168.0.101:test.c test.c`

Just like `cp` command.

# Environment Settings

## Tool installation

```shell
sudo apt-get install gcc-7
sudo apt-get install vim
```



