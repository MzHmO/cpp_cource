FROM ubuntu:18.04

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    git \
    curl \
    net-tools \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /home/box/final

COPY . .

RUN mkdir -p build && cd build && \
    cmake .. && \
    make && \
    cp final .. && \
    cd .. && rm -rf build

RUN chmod +x /home/box/final/final

EXPOSE 8080

CMD ["/home/box/final/final", "-h", "0.0.0.0", "-p", "8080", "-d", "www"]
