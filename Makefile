all:
	gcc dockerit.c -o dockerit
	sudo install dockerit /dockerit
	sudo chmod +s /dockerit
	grep dockerit /etc/ssh/sshd_config > /dev/null || printf "\nMatch Group dockerit\n  ForceCommand /dockerit\n" >> /etc/ssh/sshd_config
	systemctl reload sshd
