#include <atomic>
#include <queue>
#include <vector>
#include <optional>
#include <mutex>
#include <filesystem>
#include <thread>

struct Polyhedron {};

struct Rectangle {};

struct Box {};

struct Config {
private:
    Polyhedron hole_;
    Polyhedron plug_;
    uint8_t thread_count_;
    uint32_t hole_iteration_limit_;
    uint32_t plug_iteration_limit_;
    uint8_t projection_resolution_;
    uint8_t rotation_resolution_;
    std::filesystem::path path_;
};

struct RectangleQueue {
private:
    std::queue<Rectangle> queue_;

public:
    bool empty() const;

    void push(const Rectangle& rectangle);

    std::optional<Rectangle> pop();
};

struct BoxQueue {
private:
    std::queue<Box> queue_;
    std::mutex mutex_;

public:
    void push(const Box& box);

    std::optional<Box> pop();

    std::vector<Box> flush();
};

struct TerminalBox {
private:
    Box box_;
    std::vector<Rectangle> rectangles_;

public:
    size_t size() const;

    std::vector<uint8_t> to_bytes() const;

    TerminalBox from_bytes(const std::vector<uint8_t>& bytes);
};

struct TerminalBoxQueue {
private:
    std::queue<TerminalBox> queue_;
    std::mutex mutex_;
    size_t size_;

public:
    size_t size() const;

    void push(const TerminalBox& terminal_box);

    std::vector<TerminalBox> flush();
};

struct Exporter {
private:
    const Config& config_;

public:
    void export_terminal_boxes(const std::vector<TerminalBox>& terminal_boxes);

    void export_boxes(const std::vector<Box>& boxes);
};

struct BoxProcessor {
private:
    const Config& config_;
    BoxQueue& box_queue_;
    TerminalBoxQueue& terminal_box_queue_;

public:
    void process();
};

struct Pipeline {
private:
    BoxQueue box_queue_;
    TerminalBoxQueue terminal_box_queue_;
    Exporter exporter_;
    std::vector<std::thread> processor_threads_;
    std::thread exporter_thread_;
    std::atomic<bool> terminated_;

public:
    void start();

    void stop();
};
