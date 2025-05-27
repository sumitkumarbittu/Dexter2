FROM gcc:latest
WORKDIR /app

# Install required packages and dependencies
RUN apt-get update && apt-get install -y wget

# Download httplib.h
RUN wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h

# Copy source files
COPY . .

# Compile the C++ application
RUN g++ -o server server.cpp

EXPOSE 8080
CMD ["./server"]
