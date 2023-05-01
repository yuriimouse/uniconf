FROM centos:7
# Compilators and make tools
RUN yum groupinstall -y Development tools
# Install epel
RUN yum install -y epel-release
# Troubleshooting and debugging utilities
RUN yum install -y valgrind gdb strace gdb-gdbserver
# Convenience utilities
RUN yum install -y vim bash-completion w3m telnet nc libcurl-devel bc
#### Source dependable on ####
# cJSON
RUN git clone https://github.com/DaveGamble/cJSON.git && cd cJSON && make && make install && cp *.a /usr/local/lib/
# Libconfig
RUN yum install texinfo -y
RUN git clone https://github.com/hyperrealm/libconfig.git && cd libconfig && autoreconf || autoreconf && ./configure && make && make install
# Install CUnit of needed version
RUN yum install -y CUnit-devel
#
RUN export LD_LIBRARY_PATH=/usr/local/custom/lib/
