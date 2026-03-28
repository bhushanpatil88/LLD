/*
Design a parking system


Ask questions like, 

1. Do we support multiple vehicle types?
2. Do we need per-level statistics or just total?
3. How complex is pricing? hourly? and different for different cars
4. Are there multiple entry/exit gates?-> NO

We will design multi-level parking lot
1. Supports Motorcycle, Car, Truck.
2. Assign a suitable spot on entry
3. Compute fees on exit
4. Show availability by types


What I will implement?
Park vehicle
string parkVehicle(string& numberPlate, VehicleType type);
return ticketID

Unpark Vehicle
double unparkVehicle(string& ticketId);
int getAvailability(VehicleType type);


Classes and their attributes
Vehicle
- numberPlate
- type

ParkingSlot
- id
- level
- occupied : bool
- slotType

ParkingLevel
- id
- parkingSpots

ParkingLot
- id
- parkingLevels

Ticket
- id
- numberPlate
- parkingSpotId
- parkingLevelId

Design pattern

We will have strategy design pattern for pricing

Use singleton design pattern for parkingLot

to select the type of strategy instead of if/else we
can have factory design pattern


*/


#include<bits/stdc++.h>
using namespace std;

enum class VehicleType {
    MOTORCYCLE,
    CAR,
    TRUCK
};

enum class SpotType {
    SMALL,
    MEDIUM,
    LARGE
};

class Vehicle {
private:
    string numberPlate;
    VehicleType type;

public:
    Vehicle(string plate, VehicleType t) {
        numberPlate = plate;
        type = t;
    }

    VehicleType getType() { return type; }
    string getPlate() { return numberPlate; }
};


class ParkingSpot {
private:
    int id;
    SpotType type;
    bool occupied;
    Vehicle* vehicle;

public:
    ParkingSpot(int id, SpotType t) {
        this->id = id;
        type = t;
        occupied = false;
        vehicle = nullptr;
    }

    bool isAvailable() {
        return !occupied;
    }

    bool canFitVehicle(VehicleType vType) {
        if(vType == VehicleType::MOTORCYCLE && type==SpotType::SMALL)
            return true;

        if(vType == VehicleType::CAR && type == SpotType::MEDIUM)
            return true;

        if(vType == VehicleType::TRUCK && type == SpotType::LARGE)
            return true;

        return false;
    }

    void park(Vehicle* v) {
        vehicle = v;
        occupied = true;
    }

    void unpark() {
        vehicle = nullptr;
        occupied = false;
    }
};


class Ticket {
public:
    string ticketId;
    Vehicle* vehicle;
    ParkingSpot* spot;
    time_t entryTime;

    Ticket(string id, Vehicle* v, ParkingSpot* s) {
        ticketId = id;
        vehicle = v;
        spot = s;
        entryTime = time(nullptr);
    }
};



class ParkingFloor {
private:
    int floorNumber;
    vector<ParkingSpot*> spots;

public:
    ParkingFloor(int num) {
        floorNumber = num;
    }

    void addSpot(ParkingSpot* spot) {
        spots.push_back(spot);
    }

    ParkingSpot* getAvailableSpot(VehicleType type) {
        for(auto spot : spots) {
            if(spot->isAvailable() && spot->canFitVehicle(type))
                return spot;
        }
        return nullptr;
    }

    int getAvailability(VehicleType type) {
        int count = 0;
        for(auto spot : spots)
            if(spot->isAvailable() && spot->canFitVehicle(type))
                count++;
        return count;
    }
};


class PricingStrategy {
public:
    virtual double calculateFee(time_t entry, time_t exit) = 0;
};

class CarPricing : public PricingStrategy {
public:
    double calculateFee(time_t entry, time_t exit) {
        double hours = difftime(exit, entry) / 3600;
        return ceil(hours) * 20;
    }
};

class BikePricing : public PricingStrategy {
public:
    double calculateFee(time_t entry, time_t exit) {
        double hours = difftime(exit, entry) / 3600;
        return ceil(hours) * 10;
    }
};


class TruckPricing : public PricingStrategy {

public:
    double calculateFee(time_t entry, time_t exit) {
        double hours = difftime(exit, entry) / 3600;
        return ceil(hours) * 50;
    }
};


class PricingFactory {
public:
    static PricingStrategy* getStrategy(VehicleType type) {

        if(type == VehicleType::CAR)
            return new CarPricing();

        if(type == VehicleType::MOTORCYCLE)
            return new BikePricing();

        return new TruckPricing();
    }
};


class ParkingLot {
private:
    static ParkingLot* instance;
    vector<ParkingFloor*> floors;
    unordered_map<string, Ticket*> activeTickets;

    ParkingLot() {}

public:

    void addFloor(ParkingFloor* floor){
        floors.push_back(floor);
    }

    static ParkingLot* getInstance() {
        if(instance == nullptr) {
            instance = new ParkingLot();
        }
        return instance;
    }

    string parkVehicle(string numberPlate, VehicleType type) {

        Vehicle* vehicle = new Vehicle(numberPlate, type);

        for(auto floor : floors) {

            ParkingSpot* spot = floor->getAvailableSpot(type);

            if(spot) {
                spot->park(vehicle);

                string ticketId = to_string(rand());

                Ticket* ticket = new Ticket(ticketId, vehicle, spot);

                activeTickets[ticketId] = ticket;

                return ticketId;
            }
        }

        throw runtime_error("Parking Full");
    }


    double unparkVehicle(string ticketId) {

        Ticket* ticket = activeTickets[ticketId];

        time_t exitTime = time(nullptr);

        // PricingStrategy* strategy;

        // switch(ticket->vehicle->getType()) {

        //     case VehicleType::CAR:
        //         strategy = new CarPricing();
        //         break;

        //     case VehicleType::MOTORCYCLE:
        //         strategy = new BikePricing();
        //         break;

        //     case VehicleType::TRUCK:
        //         strategy = new TruckPricing();
        //         break;
        // }

        PricingStrategy* strategy = PricingFactory::getStrategy(ticket->vehicle->getType());

        double fee = strategy->calculateFee(ticket->entryTime, exitTime);

        ticket->spot->unpark();

        activeTickets.erase(ticketId);

        return fee;
    }

    int getAvailability(VehicleType type) {

        int total = 0;

        for(auto floor : floors)
            total += floor->getAvailability(type);

        return total;
    }

};


ParkingLot* ParkingLot::instance = nullptr;


int main() {

    // Get singleton instance
    ParkingLot* lot = ParkingLot::getInstance();

    // Create floor
    ParkingFloor* floor1 = new ParkingFloor(1);

    // Add spots
    floor1->addSpot(new ParkingSpot(1, SpotType::SMALL));
    floor1->addSpot(new ParkingSpot(2, SpotType::MEDIUM));
    floor1->addSpot(new ParkingSpot(3, SpotType::MEDIUM));
    floor1->addSpot(new ParkingSpot(4, SpotType::LARGE));

    lot->addFloor(floor1);

    cout << "Initial Availability\n";
    cout << "Motorcycle: " << lot->getAvailability(VehicleType::MOTORCYCLE) << endl;
    cout << "Car: " << lot->getAvailability(VehicleType::CAR) << endl;
    cout << "Truck: " << lot->getAvailability(VehicleType::TRUCK) << endl;

    cout << "\nParking Vehicles\n";

    string t1 = lot->parkVehicle("MH12AB1234", VehicleType::CAR);
    cout << "Car parked. Ticket: " << t1 << endl;

    string t2 = lot->parkVehicle("MH12XY1111", VehicleType::MOTORCYCLE);
    cout << "Bike parked. Ticket: " << t2 << endl;

    string t3 = lot->parkVehicle("MH14TR9999", VehicleType::TRUCK);
    cout << "Truck parked. Ticket: " << t3 << endl;

    cout << "\nAvailability After Parking\n";
    cout << "Motorcycle: " << lot->getAvailability(VehicleType::MOTORCYCLE) << endl;
    cout << "Car: " << lot->getAvailability(VehicleType::CAR) << endl;
    cout << "Truck: " << lot->getAvailability(VehicleType::TRUCK) << endl;

    // simulate parking time
    this_thread::sleep_for(chrono::seconds(3));

    cout << "\nUnparking Vehicles\n";

    double fee1 = lot->unparkVehicle(t1);
    cout << "Car fee: " << fee1 << endl;

    double fee2 = lot->unparkVehicle(t2);
    cout << "Bike fee: " << fee2 << endl;

    double fee3 = lot->unparkVehicle(t3);
    cout << "Truck fee: " << fee3 << endl;

    cout << "\nAvailability After Unparking\n";
    cout << "Motorcycle: " << lot->getAvailability(VehicleType::MOTORCYCLE) << endl;
    cout << "Car: " << lot->getAvailability(VehicleType::CAR) << endl;
    cout << "Truck: " << lot->getAvailability(VehicleType::TRUCK) << endl;

    return 0;
}