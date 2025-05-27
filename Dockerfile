FROM gcc:latest

# Install git
RUN apt-get update && apt-get install -y git

# Clone header-only libraries
RUN git clone https://github.com/yhirose/cpp-httplib.git && \
    git clone https://github.com/nlohmann/json.git

# Copy your source and HTML files
COPY server.cpp .
COPY index.html .

# Compile using the libraries
RUN g++ -std=c++17 server.cpp -o server -Icpp-httplib -Inlohmann/json/single_include

EXPOSE 8080

CMD ["./server"]
