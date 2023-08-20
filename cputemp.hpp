#include <set>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
namespace cputemp_cpp{
struct Unit { 
    std::string name;
    std::string path;
    double temp;
    std::string readed;
};

class Reader{
  private:
    std::set<std::string> correct_names = {
        "coretemp",
        "cpu_thermal"
    };
    std::vector<Unit> units;
    std::string hwmDir (int &serial){
        return (
            "/sys/class/hwmon/hwmon"+
            std::to_string(serial)
        );
    };
    std::string hwmNameFile(int &serial){
        return (
            this->hwmDir(serial)+
            "/name"
        );
    };
    std::string hwmTempFile(
      std::string &dir,
      int &serial
    ){
        return (
            dir+
            "/temp"+
            std::to_string(serial)+
            "_input"
        );
    };
    std::string hwmNameRead(int &serial){
        std::string name;
        std::ifstream name_file;
        name_file.open(this->hwmNameFile(serial));
        std::getline(name_file, name);
        name_file.close();
        return name;
    };
    bool hwmDirCheck(int &serial){
        if(
            this->correct_names.find(this->hwmNameRead(serial)) != this->correct_names.end()
        )
            return true;
        return false;
    };
    void hwmTempDiscovery(int &serial){
        std::string dir = this->hwmDir(serial);
        for (int i{0}; i < 100; i++){
            if(fs::exists(this->hwmTempFile(dir, i)))
                this->add(dir, i);
        }
    };
    void add(std::string &dir, int &serial){
        Unit next; 
        next.path = this->hwmTempFile(dir, serial);
        next.name = next.path;
        this->units.push_back(next);
    };
    void read(int &serial){
        std::string readed;
        std::ifstream temp_file;
        temp_file.open(this->units[serial].path);
        std::getline(temp_file, readed);
        this->units[serial].temp = (std::stod(readed) / 1000.00);
        this->units[serial].readed = readed;
        temp_file.close();
    };
  public:
    void discovery(){
        this->discovery(256);
    };
    void discovery(int end){
        for (int i{0}; i < end; i++)
            if(fs::exists(this->hwmDir(i))){
                if(this->hwmDirCheck(i))
                    this->hwmTempDiscovery(i);
            } else {
                break;
            }
    };
    void refresh(){
        for (int i{0}; i < this->units.size(); i++)
            this->read(i);
    };
    std::vector<Unit> get(int elem){
        std::vector<Unit> temps;
        temps.push_back(this->units[elem]);
        return temps;
    };
    std::vector<Unit> get(int start, int end){
        std::vector<Unit> temps;
        for (int i{start}; i < this->units.size(); i++)
            temps.push_back(this->units[i]);
        return temps;
    };
    std::vector<Unit> getAll(){
        return this->units;
    };
    std::vector<double> tempAll(){
        std::vector<double> temps;
        for (int i{0}; i < this->units.size(); i++)
            temps.push_back(this->units[i].temp);
        return temps;
    };
};
}
