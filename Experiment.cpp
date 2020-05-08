#include <iostream>

class Experiment {
public:
    Experiment() : val(0), id(counter++), data(new int(0)) {
        std::cout << "using Experiment() constructor: " << *this << std::endl;
    }

    explicit Experiment(int v) : val(v), id(counter++), data(new int(v)) {
        std::cout << "using explicit Experiment(int) constructor: " << *this << std::endl;
    }

    explicit Experiment(const char *l) : val(*l), id(counter++), data(new int((int)*l)) {
        std::cout << "using explicit Experiment(const char *) constructor: " << *this << std::endl;
    }

    Experiment(char t) : val(t + 2), id(counter++), data(new int((int)t)) {
        std::cout << "using Experiment(char) constructor: " << *this << std::endl;
    }

    // copy constructor, const
    Experiment(const Experiment &orginial) : val(orginial.val), id(counter++), data(new int(*orginial.data)) {
        // copy the content in a new place (see above) but no need to freed source
        std::cout << "using Experiment(const Experiment&) constructor, from " << orginial << std::endl
                  << "\tto " << *this << std::endl;
    }

    // move constructor, non const for memory management
    Experiment(Experiment &&orginial) : val(orginial.val), id(orginial.id), data(orginial.data) {
        std::cout << "using Experiment(const Experiment&&) constructor, from " << orginial << std::endl;
        orginial.data = nullptr; // re-use the same space! => use nullptr so it isn't freed with the source
    }

    ~Experiment() {
        std::cout << "deleting " << *this << std::endl;
        delete data; // deleting nullptr is safe!
    }

    // copy assignement, const
    Experiment &operator=(const Experiment &src) {
        if (this == &src) {
            std::cerr << "useless assigment!" << std::endl;
            return *this;
        }
        std::cout << "copying " << src << " into " << *this << std::endl;

        val = src.val;

        // copy value, memory should already be there (because init has been called)
        // and should not be freed (src may continue to live)
        if (src.data != nullptr)
            *data = *src.data;
        else
            std::cerr << "tried to copy invalid data, should be an error but hey" << std::endl;

        return *this;
    }

    // move assignement, non const for memory management !
    Experiment &operator=(Experiment &&src) {
        // can this == &src ? actually yes, what to do ? https://stackoverflow.com/questions/9322174/move-assignment-operator-and-if-this-rhs
        if (this == &src) {
            std::cerr << "useless move assigment!" << std::endl;
            return *this;
        }
        std::cout << "moving " << src << " into " << *this << std::endl;
        val = src.val;
        delete data;        // free previously used memory
        data = src.data;    // re-use memory
        src.data = nullptr; // remove reference to avoid freeing it
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &out, const Experiment &e);

private:
    int val;
    int *data;
    const int id;
    static int counter;
};

int Experiment::counter = 0;

Experiment get2();

std::ostream &operator<<(std::ostream &out, const Experiment &e) {
    return out << "Experiment with val= " << e.val << ", id=" << e.id << " and data_add=" << e.data;
}

int main() {
    Experiment e1(0);
    Experiment e2 = '1'; // conversion constructor
    // Experiment e3 = "1"; // error if constructor is explicit, fine otherwise
    Experiment e4(e1); // copy constructor
    // first explicit Experiment(int) then nothing, even with -O0 and -O1, due to NRVO (not guaranteed)
    // use -fno-elide-constructors to prevent return value optimization and force copy/move
    // see copy elision https://en.cppreference.com/w/cpp/language/copy_elision
    Experiment e6(get2());
    // first explicit Experiment(int), then Experiment(Experiment&&), then delete of source
    Experiment e7((Experiment &&) get2());
    std::cout << e6 << std::endl;
    e2 = e4;
    e6 = std::move(e1);
}

Experiment get2() {
    return Experiment(2);
}
