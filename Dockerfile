# syntax=docker/dockerfile:1
FROM ubuntu:24.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    python3 \
    python3-pip \
    python3-venv \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set up working directory
WORKDIR /app

# Copy project files
COPY . .

# Create venv and install Conan
RUN python3 -m venv .venv \
    && . .venv/bin/activate \
    && pip install --upgrade pip \
    && pip install conan

# Detect default Conan profile
RUN . .venv/bin/activate && conan profile detect --force

# Install dependencies and build
RUN . .venv/bin/activate \
    && conan install . --build=missing -s build_type=Release \
    && cmake --preset conan-release \
    && cmake --build --preset conan-release

# Default command: run tests with JUnit XML output
CMD ["/bin/bash", "-c", ". .venv/bin/activate && ctest --preset conan-release --output-on-failure --output-junit test-results.xml"]

