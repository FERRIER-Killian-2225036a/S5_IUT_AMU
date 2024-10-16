#include <functional>
#include <iostream>
#include <mutex>
#include <ostream>
#include <thread>
#include <vector>

std::mutex mut;

void Question1() {
    // Question 1.1 :
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << std::endl;

    // Question 1.2 :
    // Pour savoir combien de task on peut lancer en simultaner avec les thread

    // Question 1.3 :
    // J'ai 12 coeurs
}

void fctQ1() {
    std::cout << "Hello world" << std::endl;
    std::cout << "Bonjour" << std::flush;
    std::cout << " tout" << std::flush << " le monde" << std::endl;
}

void Question2() {
    std::thread th1(fctQ1);
    std::thread th2(fctQ1);

    // Question 2.1 :
    // Car la fonction ne se termine pas et n'attend pas que l'autre est fini pour la commencer

    // Question 2.2 :
    // Attend la fin du thread pour terminer la main
    th1.join();
    th2.join();
}

void fctQ3(int i) {
    mut.lock();
    std::cout << "-----------------------------" << std::endl;
    std::cout << "Thread n°" << i << std::endl;
    std::cout << "Thread ID : " << std::this_thread::get_id() << std::endl;
    std::cout << "Bonjour" << std::flush;
    std::cout << " tout" << std::flush << " le monde" << std::endl;
    std::cout << "-----------------------------" << std::endl;
    mut.unlock();
}

void Question3() {
    std::thread th1(fctQ3, 1);
    std::thread th2(fctQ3, 2);
    th1.join();
    th2.join();
    // Des fois c'est le thread 1 qui va afficher en premier et des fois c'est le tread 2
}

void Question4() {
    std::thread th1(fctQ3, 1);
    std::thread th2(fctQ3, 2);
    th1.join();
    th2.join();
    std::cout << "Thread 1 ID : " << th1.get_id() << std::endl;
    std::cout << "Thread 2 ID : " << th2.get_id() << std::endl;
    // Question 4.2 :
    // Il affiche l'erreur "thread::id of a non-executing thread"
}

void fctQ5(int i) {
    std::lock_guard<typename std::mutex> lock(mut);
    std::cout << "-----------------------------" << std::endl;
    std::cout << "Thread n°" << i << std::endl;
    std::cout << "Thread ID : " << std::this_thread::get_id() << std::endl;
    std::cout << "Bonjour" << std::flush;
    std::cout << " tout" << std::flush << " le monde" << std::endl;
    std::cout << "-----------------------------" << std::endl;
    //mut.unlock();
}

void Question5() {
    std::thread th1(fctQ5, 1);
    std::thread th2(fctQ5, 2);
    th1.join();
    th2.join();
}

unsigned i(0);
void inc() {
    std::lock_guard<typename std::mutex> lock(mut);
    ++i;
    std::cout << i << std::endl;
}

void Question6() {
    std::thread th1(inc);
    std::thread th2(inc);
    std::thread th3(inc);
    th1.join();
    th2.join();
    th3.join();
    // Question 6.1 :
    // On constate que la console affiche 1,3,2 ou 1,2,3
}

void Question6_2() { // Réponse a la question 7.2 aussi
    const int num_threads = 2000;
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread(inc));
    }
    for (auto& th : threads) {
        th.join();
    }
    std::cout << "Valeur finale du compteur: " << i << std::endl;
}

void inc7(unsigned & i) {
    std::lock_guard<typename std::mutex> lock(mut);
    ++i;
    std::cout << "i = " << i << std::endl;
}

void Question7() {
    unsigned i (0);
    std::thread th1 (inc7, std::ref(i));
    std::thread th2 (inc7, std::ref(i));
    std::thread th3 (inc7, std::ref(i));
    th1.join();
    th2.join();
    th3.join();

    // Question 7.1 :
    // On constante que le résultat des deux thread est 2
    // Pour quatre thread le résultat sera 3 pour 2 thread et 2 pour le dernier ou 3 pour tous
}

int main() {
    Question7();
    return 0;
}
