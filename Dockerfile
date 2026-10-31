# Use official Ubuntu image as base for C++ development
FROM ubuntu:22.04

# Avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install essential build tools, CMake, and SQLite3
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory inside container
WORKDIR /app

# Copy all project files into the container
COPY . .

# Build the C++ project using CMake
RUN mkdir -p build && cd build && cmake .. && make -j$(nproc)

# Expose port 8080 for web traffic
EXPOSE 8080

# Run the compiled C++ backend binary
CMD ["./build/smartcare_backend"]
