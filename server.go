package main

// Simple webserver that responds to http request on port 80.

// Based on web server code in https://golang.org/doc/articles/wiki/

import (
    "fmt"
    "log"
    "net/http"
)

func handler(w http.ResponseWriter, r *http.Request) {
    fmt.Fprintf(w, "Successfully serving on port 80\n")
}

func main() {
    http.HandleFunc("/", handler)
    log.Fatal(http.ListenAndServe(":80", nil))
}
