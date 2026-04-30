#ifndef UTILS_TIMING_H
#define UTILS_TIMING_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace MUSE
{

/**
 * @class TimingLogger
 * @brief Utility class for measuring and logging computational times.
 * 
 * This class provides functionality to:
 * - Measure elapsed time between checkpoints
 * - Print timing information to console
 * - Export timing data to CSV file
 * 
 * @example
 * TimingLogger timer("computation");
 * timer.start();
 * // ... perform computation ...
 * timer.stop("data_loading");
 * 
 * timer.start();
 * // ... perform more computation ...
 * timer.stop("simulation");
 * 
 * timer.print_summary();          // Print to console
 * timer.export_to_csv("timing.csv"); // Export to CSV
 */
class TimingLogger
{
private:
    struct TimingRecord
    {
        std::string name;
        double elapsed_seconds;
        std::string timestamp;
    };

    std::string app_name_;
    std::vector<TimingRecord> records_;
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point last_checkpoint_;
    bool is_running_;
    std::string output_folder_;

public:
    /**
     * @brief Construct a new TimingLogger object
     * @param app_name Name of the application (e.g., "compute", "vario")
     */
    TimingLogger(const std::string& app_name = "app")
        : app_name_(app_name), is_running_(false), output_folder_(".")
    {
    }

    /**
     * @brief Construct a new TimingLogger with output folder
     * @param app_name Name of the application
     * @param output_folder Path where CSV will be saved
     */
    TimingLogger(const std::string& app_name, const std::string& output_folder)
        : app_name_(app_name), is_running_(false), output_folder_(output_folder)
    {
    }

    /**
     * @brief Start the overall timing
     */
    void start()
    {
        start_time_ = std::chrono::steady_clock::now();
        last_checkpoint_ = start_time_;
        is_running_ = true;
        
        std::cout << "[" << app_name_ << "] Timing started..." << std::endl;
    }

    /**
     * @brief Record a checkpoint with optional label
     * @param label Name/label for this checkpoint
     */
    void checkpoint(const std::string& label = "")
    {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - last_checkpoint_;
        
        std::string checkpoint_name = label.empty() ? "checkpoint" : label;
        
        std::cout << "[" << app_name_ << "] " << checkpoint_name << ": "
                  << std::fixed << std::setprecision(3) << elapsed.count() << " s" << std::endl;
        
        last_checkpoint_ = now;
    }

    /**
     * @brief Stop timing and record a named phase
     * @param phase_name Name of the computational phase
     * @return Elapsed time in seconds
     */
    double stop(const std::string& phase_name)
    {
        if (!is_running_)
        {
            std::cerr << "[" << app_name_ << "] Warning: Timer was not started!" << std::endl;
            return 0.0;
        }

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - last_checkpoint_;
        
        TimingRecord record;
        record.name = phase_name;
        record.elapsed_seconds = elapsed.count();
        record.timestamp = get_current_timestamp();
        
        records_.push_back(record);
        
        std::cout << "[" << app_name_ << "] Phase '" << phase_name << "': "
                  << std::fixed << std::setprecision(3) << elapsed.count() << " s" << std::endl;
        std::cout << std::endl;
        
        last_checkpoint_ = now;
        
        return elapsed.count();
    }

    /**
     * @brief Stop the overall timer without recording a phase
     */
    void stop()
    {
        if (is_running_)
        {
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<double> total = now - start_time_;
            
            std::cout << "[" << app_name_ << "] Total elapsed time: "
                      << std::fixed << std::setprecision(3) << total.count() << " s" << std::endl;
            
            is_running_ = false;
        }
    }

    /**
     * @brief Print timing summary to console
     */
    void print_summary() const
    {
        std::cout << std::endl;
        std::cout << "============================================================" << std::endl;
        std::cout << "================== TIMING SUMMARY ===================" << std::endl;
        std::cout << "============================================================" << std::endl;
        std::cout << std::endl;
        
        std::cout << std::setw(30) << std::left << "Phase"
                  << std::setw(15) << std::right << "Time (s)"
                  << std::setw(15) << "Time (ms)" << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        
        double total = 0.0;
        for (const auto& record : records_)
        {
            std::cout << std::setw(30) << std::left << record.name
                      << std::setw(15) << std::right << std::fixed << std::setprecision(3) << record.elapsed_seconds
                      << std::setw(15) << std::fixed << std::setprecision(1) << record.elapsed_seconds * 1000.0 << std::endl;
            total += record.elapsed_seconds;
        }
        
        std::cout << "------------------------------------------------------------" << std::endl;
        std::cout << std::setw(30) << std::left << "TOTAL"
                  << std::setw(15) << std::right << std::fixed << std::setprecision(3) << total
                  << std::setw(15) << std::fixed << std::setprecision(1) << total * 1000.0 << std::endl;
        std::cout << std::endl;
    }

    /**
     * @brief Export timing data to CSV file
     * @param filename Output CSV filename
     * @return true if export successful, false otherwise
     */
    bool export_to_csv(const std::string& filename) const
    {
        std::string full_path = output_folder_ + "/" + filename;
        
        std::ofstream file(full_path);
        if (!file.is_open())
        {
            std::cerr << "[" << app_name_ << "] Error: Could not open file for writing: " << full_path << std::endl;
            return false;
        }

        // Write CSV header
        file << "application,phase,time_seconds,time_ms,timestamp\n";
        
        // Write data rows
        for (const auto& record : records_)
        {
            file << app_name_ << ","
                 << record.name << ","
                 << std::fixed << std::setprecision(6) << record.elapsed_seconds << ","
                 << std::fixed << std::setprecision(1) << record.elapsed_seconds * 1000.0 << ","
                 << record.timestamp << "\n";
        }

        file.close();
        
        std::cout << "[" << app_name_ << "] Timing data exported to: " << full_path << std::endl;
        
        return true;
    }

    /**
     * @brief Export timing data to CSV with custom path
     * @param full_path Full path for output CSV file
     * @return true if export successful, false otherwise
     */
    bool export_to_csv_path(const std::string& full_path) const
    {
        bool file_exists = std::filesystem::exists(full_path);

        std::ofstream file(full_path, std::ios::app); // Open in append mode
        if (!file.is_open())
        {
            std::cerr << "[" << app_name_ << "] Error: Could not open file for writing: " << full_path << std::endl;
            return false;
        }

        // Write CSV header only if file does not exist
        if (!file_exists)
        {
            file << "application,phase,time_seconds,time_ms,timestamp\n";
        }

        // Write data rows
        // i tempi salvati sono quelli registrati fino ai punti di stop() chiamati
        // sono incrementali, quindi se si chiama stop() più volte, si avranno più righe per la stessa fase (con tempi diversi)
        //il tempo stampato è la differenza tra il tempo di stop() e l'ultimo stop() o start(), quindi rappresenta il tempo trascorso per quella fase specifica
        // il tempo totale indica la somma di tutti i tempi registrati fino a quel punto, ma non è necessariamente la somma totale se si chiama stop() più volte, perché ogni stop() registra un tempo incrementale
        for (const auto& record : records_)
        {
            file << app_name_ << ","
                 << record.name << ","
                 << std::fixed << std::setprecision(6) << record.elapsed_seconds << ","
                 << std::fixed << std::setprecision(1) << record.elapsed_seconds * 1000.0 << ","
                 << record.timestamp << "\n";
        }

        file.close();
        
        std::cout << "[" << app_name_ << "] Timing data exported to: " << full_path << std::endl;
        
        return true;
    }

    /**
     * @brief Get total elapsed time
     * @return Total time in seconds
     */
    double get_total_time() const
    {
        double total = 0.0;
        for (const auto& record : records_)
        {
            total += record.elapsed_seconds;
        }
        return total;
    }

    /**
     * @brief Get number of recorded phases
     * @return Number of timing records
     */
    size_t get_num_records() const
    {
        return records_.size();
    }

    /**
     * @brief Check if timer is currently running
     * @return true if timer is running
     */
    bool is_running() const
    {
        return is_running_;
    }

    /**
     * @brief Clear all timing records
     */
    void clear()
    {
        records_.clear();
        is_running_ = false;
    }

    /**
     * @brief Set output folder for CSV export
     * @param folder Path to output folder
     */
    void set_output_folder(const std::string& folder)
    {
        output_folder_ = folder;
    }

private:
    /**
     * @brief Get current timestamp as string
     * @return Formatted timestamp
     */
    std::string get_current_timestamp() const
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        
        return ss.str();
    }
};

} // namespace MUSE

#endif // UTILS_TIMING_H