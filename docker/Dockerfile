FROM centos:7
# Install epel
RUN yum install -y epel-release
# Add sclo utility 
RUN yum install -y centos-release-scl-rh
# Compilators and make tools
RUN yum groupinstall -y Development tools
RUN yum install -y glibc-devel
# Troubleshooting and debugging utilities
RUN yum install -y valgrind gdb strace gdb-gdbserver
# Convenience utilities
RUN yum install -y vim bash-completion w3m telnet nc libcurl-devel bc
#### Source dependable on ####
# cJSON
RUN git clone https://github.com/DaveGamble/cJSON.git && cd cJSON && make && make install && cp *.a /usr/local/lib/ && cd .. && rm -rf cJSON
# Libconfig
RUN yum install texinfo -y
RUN git clone https://github.com/hyperrealm/libconfig.git && cd libconfig && autoreconf || autoreconf && ./configure && make && make install && cd .. && rm -rf libconfig
# Install CUnit of needed version
RUN yum install -y CUnit-devel
# lists
RUN git clone https://github.com/yuriimouse/lists.git && cd lists/ && make install && cd .. && rm -rf lists/
#
RUN export LD_LIBRARY_PATH=/usr/local/custom/lib/:/usr/local/lib/
