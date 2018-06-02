all:
	gcc dockerit.c -o dodckerit
	sudo install dockerit /dockerit
	grep dockerit /etc/ssh/sshd_config > /dev/null || printf "\nMatch Group dockerit\n  ForceCommand /dockerit\n" >> /etc/ssh/sshd_config
