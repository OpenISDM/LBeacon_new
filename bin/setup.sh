#!/bin/bash

# Read user input
read -p "Enter all IP addresses which you want to allow for SSH: " ssh_source_ip_addresses
read -p "Enter all IP addresses which you want to allow for ICMP (ping): " icmp_source_ip_addresses
read -p "Enter all IP addresses for BiDaE applications: " bidae_ip_addresses
read -p "Enter all UDP listening ports for BiDaE applications: " bidae_ports

echo "Please confirm following settings:" 
echo "-  IP addresses will be alloed for SSH: " $ssh_source_ip_addresses
echo "-  IP addresses will be alloed for ICMP (ping): " $icmp_source_ip_addresses
echo "-  IP addresses for BiDaE applications: " $bidae_ip_addresses
echo "-  Listening port for BiDaE application: " $bidae_ports

echo ""
read -p "Confirm and apply these settings? [Y/n]" answer

echo "---" 
if [ "_$answer" != "_Y" ] 
then
    echo "Abort the settings, and please try again."
    exit 0 
fi 

# Prohibit root from ssh
sudo sed -i 's/PermitRootLogin.*/PermitRootLogin no/' /etc/ssh/sshd_config
sudo /etc/init.d/ssh restart

# Setup iptables

# Flush all rules
sudo iptables -F
sudo iptables -X

# Set default filter policy
iptables -P INPUT DROP
iptables -P OUTPUT DROP
iptables -P FORWARD DROP

# Allow unlimited traffic on loopback
iptables -A INPUT -i lo -j ACCEPT
iptables -A OUTPUT -o lo -j ACCEPT


# Allow incoming ssh
iptables -A INPUT -p tcp -s $ssh_source_ip_addresses --sport 513:65535 --dport 22 -m state --state NEW,ESTABLISHED -j ACCEPT
iptables -A OUTPUT -p tcp -d $ssh_source_ip_addresses --sport 22 --dport 513:65535 -m state --state ESTABLISHED -j ACCEPT

# Allow incoming ICMP (ping)
iptables -A INPUT -p icmp --icmp-type 8 -s $icmp_source_ip_addresses -m state --state NEW,ESTABLISHED,RELATED -j ACCEPT
iptables -A OUTPUT -p icmp --icmp-type 0 -d $icmp_source_ip_addresses -m state --state ESTABLISHED,RELATED -j ACCEPT

# Allow outgoing ICMP (ping)
iptables -A OUTPUT -p icmp --icmp-type 8  -m state --state NEW,ESTABLISHED,RELATED -j ACCEPT
iptables -A INPUT -p icmp --icmp-type 0 -m state --state ESTABLISHED,RELATED -j ACCEPT

# Allow incoming application traffic
iptables -A INPUT -p udp -s $bidae_ip_addresses --match multiport --dports $bidae_ports -j ACCEPT
iptables -A OUTPUT -p udp -d $bidae_ip_addresses -j ACCEPT

# Make sure nothing comes or goes out of thix box
iptables -A INPUT -j DROP
iptables -A OUTPUT -j DROP
