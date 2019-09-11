# Build set_ambient
FROM gcc:9 as cbuilder

RUN set -eux; \
      apt-get update; \ 
      apt install -y libcap-ng-dev; \
      rm -rf /var/lib/apt/lists/*

COPY set_ambient.c /
RUN gcc ./set_ambient.c -o set_ambient -lcap-ng

# Build Go web server
FROM golang:1.13 as gobuilder

COPY server.go /go/
RUN go build server.go

# Build main image
FROM debian:buster

RUN set -eux; \
      apt-get update; \ 
      apt install -y libcap2-bin gosu; \
      rm -rf /var/lib/apt/lists/*

COPY --from=cbuilder /set_ambient /
RUN setcap cap_net_bind_service+p /set_ambient
COPY --from=gobuilder /go/server /

# This version of server has NET_BIND_SERVICE in the inherited set and the
# effective bit set
COPY --from=gobuilder /go/server /inh_server
RUN setcap cap_net_bind_service+ie /inh_server
