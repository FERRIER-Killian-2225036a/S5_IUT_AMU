#include <iostream>
#include <thread>
#include <semaphore>
#include <mutex>
#include <vector>
#include <chrono>

// Définition des constantes et variables:
const int capacity = 5; // capacité de l'ascenseur
int passengersNumber = 12; // nombre de passagers dans l'attente au floor 0
int actualPassengerNumber = 0; // nombre de passagers dans l'ascenseur
int passengersUnloaded = 0; // nombre de passagers sortant au floor 1

// Définitions des mutex :
std::mutex counter;

// Définitions des sémaphores :
std::counting_semaphore<capacity> loadPassenger(capacity); // Sémaphore pour gérer la montée des passagers
std::binary_semaphore unloadPassenger(0); // Signaler à un passager qu'il peut descendre
std::binary_semaphore signalDeparture(0); // Signaler à l'ascenseur qu'il peut monter/descendre

// Cas d'utilisation : Permettre à un passager de monter dans l'ascenseur
void board(int id){
    // Bloquer le mutex pour incrémenter le nombre de passagers dans l'ascenseur
    std::lock_guard<std::mutex> lock(counter);
    actualPassengerNumber++;
    std::cout << "Passenger " << id << " is boarding" << std::endl;
}

// Cas d'utilisation : Permettre à un passager de descendre de l'ascenseur
void unboard(int id){
    // Bloquer le mutex pour :
    // décrémenter le nombre de passagers dans l'ascenseur
    // décrémenter le nombre de personnes attendant au floor 0
    // incrémenter le nombre de personnes descendues au floor1 durant le trajet en cours
    std::lock_guard<std::mutex> lock(counter);
    actualPassengerNumber--;
    passengersNumber--;
    passengersUnloaded++;
    std::cout << "Passenger " << id << " is unboarding" << std::endl;
}

// Cas d'utilisation : Permettre à l'ascenseur d'attendre qu'il soit rempli ou qu'il ne reste plus de passagers
void load(){
    while(true){
        std::lock_guard<std::mutex> lock(counter);
        // Si la capacité est atteinte, ou si tous les passagers restants au floor 0 sont dans l'ascenseur
        if (actualPassengerNumber == capacity || actualPassengerNumber == passengersNumber) {
            signalDeparture.release(); // L'ascenseur peut démarrer
            return;
        }
    }
}

// Cas d'utilisation : Simuler le mouvement de l'ascenseur
void run(){
    std::cout << std::endl << "Elevator is running..." << std::endl;
    signalDeparture.acquire(); // L'ascenseur ne peut plus démarrer
}

// Cas d'utilisation : Libérer les passagers
void unload(){
    std::cout << "Floor 1" << std::endl << std::endl;
    // Nombre de passagers à décharger
    // min() prend le plus petit entre les paramètres
    int passengersToUnload = std::min(capacity, actualPassengerNumber); 
    for (int i = 0; i < passengersToUnload; ++i) {
        unloadPassenger.release();  // Autoriser les passagers à descendre
    }

    bool allUnloaded = false;
    // tant qu'il reste des passagers dans l'ascenseur
    while (!allUnloaded) {
        {
            std::lock_guard<std::mutex> lock(counter);
            // Tous les passagers montés ont été déchargés
            if (passengersUnloaded == passengersToUnload) {
                allUnloaded = true;  // Tous les passagers sont déchargés
                passengersUnloaded = 0; // Réinitialiser le nombre de passagers sortis
                actualPassengerNumber = 0; // Réinitialiser le nombre de passagers dans l'ascenseur
                signalDeparture.release(); // Autoriser l'ascenseur à redescendre
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Petit délai pour éviter une boucle rapide
    }
}

// Cas d'utilisation : Cycle de vie d'un passager
void passenger(int id){
    loadPassenger.acquire(); // Attendre que l'ascenseur soit prêt
    board(id);  // Monter dans l'ascenseur
    unloadPassenger.acquire(); // Attendre pour descendre
    unboard(id);  // Descendre de l'ascenseur
}

// Cas d'utilisation : Cycle de vie de l'ascenseur
void elevator(){
    while(passengersNumber > 0){
        load();   // Charger les passagers
        run();    // Monter à l'étage 1
        unload(); // Décharger les passagers
        run();    // Redescendre à l'étage 0
        std::cout << "Floor 0" << std::endl << std::endl;

        // Calculer le nombre de passagers restants à charger
        int remainingPassengers = std::min(capacity, passengersNumber);
        loadPassenger.release(remainingPassengers); // Libérer les places pour le prochain groupe
    }
}

int main(){

    // Initialisation de l'ascenseur :
    std::thread elevatorThread(elevator);

    // Initialisation des passagers :
    std::vector<std::thread> passengersThread;
    for(int i = 0; i < passengersNumber; ++i){
        passengersThread.emplace_back(passenger, i + 1);
    }

    elevatorThread.join();
    for(auto& passenger : passengersThread){
        passenger.join();
    }

    return 0;
}
