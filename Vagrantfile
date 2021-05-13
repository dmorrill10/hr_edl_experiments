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
    # To build Singularity images to run experiments on other machines within a container.
    sudo apt-get install -y \
      build-essential \
      libssl-dev \
      uuid-dev \
      libgpgme11-dev \
      squashfs-tools \
      libseccomp-dev \
      pkg-config \
      cryptsetup
    SING_VERSION="v3.7.3"

    cd /tmp
    export VERSION=1.14.12 OS=linux ARCH=amd64 && \  # Replace the values as needed
    wget https://dl.google.com/go/go$VERSION.$OS-$ARCH.tar.gz && \ # Downloads the required Go package
    sudo tar -C /usr/local -xzvf go$VERSION.$OS-$ARCH.tar.gz && \ # Extracts the archive
    rm go$VERSION.$OS-$ARCH.tar.gz -f   # Deletes the ``tar`` file

    echo 'export GOPATH=${HOME}/go' >> ~/.bashrc
    echo 'export PATH=/usr/local/go/bin:${PATH}:${GOPATH}/bin' >> ~/.bashrc
    export GOPATH=${HOME}/go
    export PATH=/usr/local/go/bin:${PATH}:${GOPATH}/bin

    mkdir -p $GOPATH/src/github.com/sylabs
    cd $GOPATH/src/github.com/sylabs
    git clone https://github.com/sylabs/singularity.git
    cd singularity
    git checkout ${SING_VERSION}

    ./mconfig && \
    sudo make -C builddir && \
    sudo make -C builddir install

    # apt-get install python3-dev -y  # For python OpenSpiel interface.
    # apt-get install gdb valgrind -y  # For debugging C++.
  SHELL
end

