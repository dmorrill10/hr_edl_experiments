# -*- mode: ruby -*-
# vi: set ft=ruby :

# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/focal64"

  # TODO: Share your hr_edl directory with the VM.
  # config.vm.synced_folder '~/host/hr_edl_experiments', '/hr_edl_experiments'

  # Disable automatic box update checking.
  config.vm.box_check_update = false

  config.vm.provider "virtualbox" do |vb|
    # Customize the amount of memory on the VM:
    vb.memory = "10000"
    vb.cpus = 4
  end
  config.vm.provision "shell", inline: <<-SHELL
    apt-get update -y
    apt-get install bash make clang unzip git wget python3 -y
    snap install cmake --classic
    # Optional
    # apt-get install python3-dev -y  # For python interfaces to experiment code.
    # apt-get install gdb valgrind -y  # For debugging C++.

    # To build Singularity images to run experiments on other machines within a container.
    # apt-get install singularity -y
  SHELL
end

