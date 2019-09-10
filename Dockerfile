FROM debian:stable

RUN set -eux; \
      apt-get update; \ 
      apt install -y libcap2-bin gosu; \
      rm -rf /var/lib/apt/lists/*

COPY set_ambient /
# COPY doesn't seem to respect file attributes, so need to set again
RUN setcap cap_net_bind_service+p /set_ambient
COPY server /

# This version of server has NET_BIND_SERVICE in the inherited set and the
# effective bit set
COPY server /inh_server
RUN setcap cap_net_bind_service+ie /inh_server
