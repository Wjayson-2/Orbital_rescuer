//
// Created by WJays on 2026/8/3.
//
#include "MissionConfig.hpp"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

bool checkConfig(const MissionConfig& config);

MissionConfig config_global;

void loadMissionConfig(
    const std::string& filename)
{
    MissionConfig config;
    std::ifstream file(filename);

    std::cout<<filename<<"\n";
    if(!file.is_open())
    {
        throw std::runtime_error("File not found");
    }


    std::string key;


    while(file >> key)
    {
        if(key == "name")
        {
            file >> config.name;
            if(file.fail())
            {
                std::cout << "Name invalid\n";
            }else {
                config.name_initialized = true;
            }
        }


        else if(key == "callsign")
        {
            file >> config.callsign;
            if(file.fail())
            {
                std::cout << "Callsign invalid\n";
            }else {
                config.callsign_initialized = true;
            }
        }

        else if(key == "startPosition")
        {
            file >> config.startPosition.x;
            file >> config.startPosition.y;
            if(file.fail() || abs(config.startPosition.x) > 1000.0 || abs(config.startPosition.y) > 700.0)
            {
                std::cout << "Start position wrong\n";
            }else{
                config.pos_initialized = true;
            };
        }

        else if(key == "startVelocity")
        {

            file >> config.startVelocity.x;
            file >> config.startVelocity.y;
            if(file.fail())
            {
                std::cout << "Startvelocity invalid\n";
            }else {
                config.velocity_initialized = true;
            }
        }

        else if(key == "startAngleRadians")
        {
            file >> config.startAngleRadians;
            if(file.fail())
            {
                std::cout << "Start angle invalid\n";
            }else {
                config.angle_initialized = true;
            }
        }

        else if(key == "startFuel")
        {
            file >> config.startFuel;
            if(file.fail())
            {
                std::cout << "Startfuel invalid\n";
            }else {
                config.fuel_initialized = true;
            }
        }

        else if(key == "timeLimit")
        {
            file >> config.timeLimit;
            if(file.fail() || config.timeLimit <= 0)
            {
                std::cout << "Timelimit invalid\n";
            }else {
                config.time_initialized = true;
            }
        }else if (key == "angularVelocity") {
            file >> config.angularVelocity;
            if(file.fail()) {
                std::cout << "Angular velocity invalid\n";
            }else{
                config.angularVelocity_initialized = true;
            }
        }else if (key == "mass") {
            file >> config.mass;
            if(file.fail()) {
                std::cout << "Mass invalid\n";
            }else{
                config.mass_initialized = true;
            }
        }else if (key == "hullIntegrity") {
            file >> config.hullIntegrity;
            if(file.fail() || config.hullIntegrity > 100 || config.hullIntegrity <= 0) {
                std::cout << "Hull integrity invalid\n";
            }else{
                config.hullIntegrity_initialized = true;
            }
        }else if (key == "DockingLimits") {
            file >> config.dockingLimits.captureDistance;
            file >> config.dockingLimits.safeSpeed;
            file >> config.dockingLimits.safeAngleRadians;
            if(file.fail() || config.dockingLimits.captureDistance <= 0 || config.dockingLimits.safeSpeed <= 0) {
                std::cout << "Docking limits invalid\n";
            }else{
                config.dockingLimits_initialized = true;
            }
        }else if(key == "planet")
        {
            file >> config.planetspec.position.x;
            file >> config.planetspec.position.y;
            file >> config.planetspec.radius;
            file >> config.planetspec.mass;

            if(file.fail())
            {
                std::cout << "Planet wrong\n";
            }else{
                config.planetspec_initialized = true;
            };
        }

        else if(key[0] == '#') {
            std::string ignored;
            std::getline(file, ignored);
            continue;
        }
        else {
            cout<<"Error at: "<<key<<endl;
            throw std::runtime_error("Unknown key");

        }
    };

    if (checkConfig(config)) {
        config_global = config;
    }else{
        throw std::runtime_error("Invalid config");
    }
}

bool checkConfig(const MissionConfig& config) {
    if (!config.time_initialized) {
        cout<<"Time limit missing\n";
        return false;
    }
    if (!config.mass_initialized) {
        cout<<"Mass missing\n";
        return false;
    }
    if (!config.pos_initialized) {
        cout<<"Start position missing\n";
        return false;
    }
    if (!config.name_initialized) {
        cout<<"Name missing\n";
        return false;
    }
    if (!config.callsign_initialized) {
        cout<<"Callsign missing\n";
        return false;
    }
    if (!config.velocity_initialized) {
        cout<<"Velocity missing\n";
        return false;
    }
    if (!config.fuel_initialized) {
        cout<<"Fuel missing\n";
        return false;
    }
    if (!config.angle_initialized) {
        cout<<"Angle missing\n";
        return false;
    }
    if (!config.angularVelocity_initialized) {
        cout<<"Angular velocity missing\n";
        return false;
    }
    if (!config.hullIntegrity_initialized) {
        cout<<"Hull integrity missing\n";
        return false;
    }
    if (!config.dockingLimits_initialized) {
        cout<<"Docking limits missing\n";
        return false;
    }
    if (!config.planetspec_initialized) {
        cout<<"Planet spec missing\n";
        return false;
    }
    return true;
}
