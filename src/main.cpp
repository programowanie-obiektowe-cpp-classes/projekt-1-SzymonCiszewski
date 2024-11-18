#include "RandomNameGenerator.hpp"
#include "print"
#include "string"
#include <iostream>
#include <variant>

class Pracownik
{
protected:
    Pracownik(std::string imie) : imie_{imie} {};
    std::string get_imie() const { return imie_; };

private:
    std::string imie_;
};

class Inzynier : public Pracownik
{
public:
    Inzynier() : Pracownik{getRandomName()}, wydzial_{"Mel"} {}
    Inzynier(std::string wydzial) : Pracownik{getRandomName()}, wydzial_{wydzial} {}
    void print() const { std::println("Inzynier {} po wydziale {}", get_imie(), wydzial_); }
    int  get_C() const { return CI_; }
    int  get_salary() const { return salaryI_; }

private:
    static const int CI_      = 100;
    static const int salaryI_ = 7000;
    std::string      wydzial_;
};

class Magazynier : public Pracownik
{
public:
    Magazynier() : Pracownik{getRandomName()}, obsl_widl_{0} {}
    Magazynier(bool obsl_widl) : Pracownik{getRandomName()}, obsl_widl_{obsl_widl} {}

    void print() const { std::println("Magazynier {} obslugujacy wozek widlowy: {}", get_imie(), obsl_widl_); }
    int  get_C() const { return CMag_; }
    int  get_salary() const { return salaryMag_; }

private:
    static const int CMag_      = 50;
    static const int salaryMag_ = 4000;

    bool obsl_widl_;
};

class Marketer : public Pracownik
{
public:
    Marketer() : Pracownik{getRandomName()}, follows_{100} {}
    Marketer(int follows) : Pracownik{getRandomName()}, follows_{follows} {}
    void print() const { std::println("Marketer {} o {} followersach", get_imie(), follows_); }
    int  get_C() const { return CMkt_; }
    int  get_salary() const { return salaryMar_; }

private:
    static const int CMkt_      = 80;
    static const int salaryMar_ = 6000;

    int follows_;
};

class Robotnik : public Pracownik
{
public:
    Robotnik() : Pracownik{getRandomName()}, but_{45} {}
    Robotnik(double but) : Pracownik{getRandomName()}, but_{but} {}
    void print() const { std::println("Robotnik {} o  rozmiarze buta: {}", get_imie(), but_); }
    int  get_C() const { return CR_; }
    int  get_salary() const { return salaryRob_; }

private:
    static const int CR_        = 10;
    static const int salaryRob_ = 3000;
    double           but_;
};

class Kredyt
{
public:
    Kredyt() = default;
    Kredyt(double dlug, int liczba_rat)
        : dlug_{dlug}, pozostale_raty_{liczba_rat}, rata_{dlug / liczba_rat + (10 + liczba_rat / 10) / 100 * dlug}
    {
        if (pozostale_raty_ > X)
        {
            pozostale_raty_ = X;
            rata_           = dlug / liczba_rat + (10 + liczba_rat / 10) / 100 * dlug;
            std::println("Przekroczono maksymalna liczbe rat. Przyjeto liczbe rat jako {} miesiace.", X);
        }
        dlug = pozostale_raty_ * rata_;
    }
    double get_rata() const { return rata_; }
    double get_dlug() const { return dlug_; }
    int    get_pozostale_raty() const { return pozostale_raty_; }
    void   splac_rate()
    {
        dlug_ -= rata_;
        pozostale_raty_--;
    }
    void print_info() const
    {
        std::println("Kredyt:dlug={}\trata={}\tpozostale raty={}", get_dlug(), get_rata(), get_pozostale_raty());
    }

private:
    double           dlug_;
    double           rata_;
    int              pozostale_raty_;
    static const int X = 60;
};

struct PrinterPracownikow
{
    template < typename T >
    void operator()(const T& pracownik) const
    {
        pracownik.print();
    }
};
struct GetSalaryPracownikow
{
    template < typename T >
    int operator()(const T& pracownik) const
    {
        return pracownik.get_salary();
    }
};
struct GetCPracownikow
{
    template < typename T >
    int operator()(const T& pracownik) const
    {
        return pracownik.get_C();
    }
};
struct WizytatorPracownikow
{
    std::string operator()(const Inzynier&) const { return "Inzynier"; };
    std::string operator()(const Magazynier&) const { return "Magazynier"; };
    std::string operator()(const Marketer&) const { return "Marketer"; };
    std::string operator()(const Robotnik&) const { return "Robotnik"; };
};

class Firma
{
public:
    Firma(double stan_konta)
        : stan_konta_{stan_konta},
          n_produktow_{0},
          wyprodukowano_w_miesiacu_{0},
          pojemnosc_magazynu_{0},
          popyt_{0},
          cena_produktu_{0},
          nr_miesiaca_{0},
          zadluzenie_{0}
    {
        Inzynier   inz1{};
        Magazynier mag1{};
        Marketer   mark1{};
        Robotnik   rob1{};
        zatrudnij(inz1);
        zatrudnij(mag1);
        zatrudnij(mark1);
        zatrudnij(rob1);
    }

    void drukuj_pracownikow() const
    {
        if (prac_.size() > 0)
        {
            for (int i = 0; i < prac_.size(); ++i)
                std::visit(PrinterPracownikow{}, prac_[i]);
        }
        else
            std::println("Brak pracownikow do wyswietlenia");
    }
    void wez_kredyt(double kwota, int czas_splaty)
    {
        Kredyt k{kwota, czas_splaty};
        if (zadluzenie_ < M * wartosc_firmy() && kwota < M * wartosc_firmy())
        {
            stan_konta_ += kwota;
            kredyty_.push_back(k);
            zadluzenie_ += kredyty_.back().get_dlug();
        }
        else
        {
            std::println("Nie mozna wziac kredyty. Przekroczono maksymalne zadluzenie.");
        }
    }
    void zatrudnij(const std::variant< Inzynier, Marketer, Magazynier, Robotnik >& pracownik)
    {
        prac_.push_back(pracownik);
        if (std::visit(WizytatorPracownikow{}, prac_.back()) == "Inzynier")
            cena_produktu_ += std::visit(GetCPracownikow{}, prac_.back());
        if (std::visit(WizytatorPracownikow{}, prac_.back()) == "Marketer")
            popyt_ += std::visit(GetCPracownikow{}, prac_.back());
        if (std::visit(WizytatorPracownikow{}, prac_.back()) == "Magazynier")
            pojemnosc_magazynu_ += std::visit(GetCPracownikow{}, prac_.back());
        if (std::visit(WizytatorPracownikow{}, prac_.back()) == "Robotnik")
            n_produktow_ += std::visit(GetCPracownikow{}, prac_.back());
    }
    void print_produkcja_info() const
    {
        std::println("Cena produktu={}\tPopyt={}\tPojemnosc magazynu={}\tLiczba wyprodukowanych produktow={}",
                     cena_produktu_,
                     popyt_,
                     pojemnosc_magazynu_,
                     wyprodukowano_w_miesiacu_);
    }
    void zaplac_wynagrodzenie()
    {
        for (int i = 0; i < prac_.size(); ++i)
        {
            stan_konta_ -= std::visit(GetSalaryPracownikow{}, prac_[i]);
        }
    }
    void otrzymaj_przychod()
    {
        wyprodukowano_w_miesiacu_ = n_produktow_;
        if (n_produktow_ > pojemnosc_magazynu_)
            wyprodukowano_w_miesiacu_ = pojemnosc_magazynu_;
        if (popyt_ < n_produktow_)
            historia_przych_.push_back(cena_produktu_ * popyt_);
        else
            historia_przych_.push_back(cena_produktu_ * wyprodukowano_w_miesiacu_);

        stan_konta_ += historia_przych_.back();
        nr_miesiaca_++;
    }
    void splac_raty()
    {
        for (int i = 0; i < kredyty_.size(); ++i)
        {
            if (kredyty_[i].get_pozostale_raty() > 0)
            {
                stan_konta_ -= kredyty_[i].get_rata();
                kredyty_[i].splac_rate();
            }
        }
    }
    void print_kredyt_info() const
    {
        for (int i = 0; i < kredyty_.size(); ++i)
            kredyty_[i].print_info();
    }
    double wartosc_firmy()
    {
        double przycho_roczny = 0;
        int    size_hp        = historia_przych_.size();
        for (int i = 0; i < N && i < size_hp; ++i)
        {
            przycho_roczny += historia_przych_[size_hp - 1 - i];
        }
        for (int i = 0; i < kredyty_.size(); ++i)
        {
            przycho_roczny -= kredyty_[i].get_dlug();
        }
        return przycho_roczny;
    }
    double get_stan_konta() const { return stan_konta_; }

private:
    double                                                                  stan_konta_;
    std::vector< Kredyt >                                                   kredyty_;
    std::vector< std::variant< Inzynier, Marketer, Magazynier, Robotnik > > prac_;
    std::vector< double >                                                   historia_przych_;
    int                                                                     n_produktow_;
    double                                                                  pojemnosc_magazynu_;
    double                                                                  popyt_;
    double                                                                  cena_produktu_;
    int                                                                     nr_miesiaca_;
    double                                                                  zadluzenie_;
    double                                                                  wyprodukowano_w_miesiacu_;
    const static int                                                        M = 10;
    const static int                                                        N = 12;
};

class Gra
{
public:
    Gra(Firma& firma, bool stan) : firma_{firma}, stan_{stan} {}
    void akcja_gracza()
    {
        std::string akcja_gracza;
        std::string atrybut_inz;
        bool        atrybut_mag;
        int         atrybut_mkt;
        double      atrybut_rob;
        double      kwota;
        int         czas_splaty;

    wczytanie_komendy:
        std::println("Podaj komende:");
        std::println("\tlp - wylistuj pracownikow");
        std::println("\tzinz - zatrudnij inzyzniera (wynagrodzenie = 7000, wartosc produktu +=10)");
        std::println("\tzmag - zatrudnij magazyniera (wynagrodzenie = 4000, pojemnosc magazynu +=50)");
        std::println("\tzmkt - zatrudnij marketera (wynagrodzenie = 6000, popyt +=80)");
        std::println("\tzrob - zatrudnij robotnika (wynagrodzenie = 3000, produkcja +=10)");
        std::println("\tkred - wez kredyt (max. czas kredytu - 60 miesiecy)");
        std::println("\tkt - zakoncz ture i wyswietl stan firmy na poczatku nastepnego miesiaca");
        std::cin >> akcja_gracza;
        if (akcja_gracza == "lp")
        {
            firma_.drukuj_pracownikow();
        }
        else if (akcja_gracza == "zinz")
        {
            std::println("podaj ukonczony wydzial");
            std::cin >> atrybut_inz;
            if (atrybut_inz.empty() || !std::all_of(atrybut_inz.begin(), atrybut_inz.end(), ::isalpha))
            {
                std::cerr << "Blad! Niepoprawne dane wejsciowe. Podaj ponownie." << std::endl;
            }
            else
            {
                Inzynier inz{atrybut_inz};
                firma_.zatrudnij(inz);
            }
        }
        else if (akcja_gracza == "zmag")
        {
            std::println("Czy umie obslugiwac wozek widlowy? (Tak - podaj 1, Nie - podaj 0)");
            std::cin >> atrybut_mag;
            if (std::cin.fail() || (atrybut_mag != 0 && atrybut_mag != 1))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
                std::cerr << "Blad! Niepoprawne dane wejsciowe. Podaj ponownie." << std::endl;
            }
            else
            {
                Magazynier mag{atrybut_mag};
                firma_.zatrudnij(mag);
            }
        }
        else if (akcja_gracza == "zmkt")
        {
            std::println("podaj liczbe followersow");
            std::cin >> atrybut_mkt;
            if (std::cin.fail() || atrybut_mkt < 0)
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
                std::cerr << "Blad! Niepoprawne dane wejsciowe. Podaj ponownie." << std::endl;
            }
            else
            {
                Marketer mkt{atrybut_mkt};
                firma_.zatrudnij(mkt);
            }
        }
        else if (akcja_gracza == "zrob")
        {
            std::println("podaj rozmiar buta");
            std::cin >> atrybut_rob;
            if (std::cin.fail() || atrybut_rob <= 0)
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
                std::cerr << "Blad! Niepoprawne dane wejsciowe. Podaj ponownie." << std::endl;
            }
            else
            {
                Robotnik rob{atrybut_rob};
                firma_.zatrudnij(rob);
            }
        }
        else if (akcja_gracza == "kred")
        {
            std::println("podaj kwote kredytu");
            std::cin >> kwota;
            if (std::cin.fail() || kwota <= 0)
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
                std::cerr << "Blad! Niepoprawne dane wejsciowe. Podaj ponownie." << std::endl;
            }
            else
            {
                std::println("podaj czas splaty [miesiace]");
                std::cin >> czas_splaty;
                if (std::cin.fail() || czas_splaty <= 0)
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
                    std::cerr << "Blad! Niepoprawne dane wejsciowe. Podaj ponownie." << std::endl;
                }
                else
                {
                    firma_.wez_kredyt(kwota, czas_splaty);
                }
            }
        }
        else if (akcja_gracza == "kt")
        {
            firma_.otrzymaj_przychod();
            firma_.zaplac_wynagrodzenie();
            firma_.splac_raty();
            firma_.print_kredyt_info();
            firma_.print_produkcja_info();
            std::println("Stan konta firmy: {}", firma_.get_stan_konta());
            std::println("Wartosc firmy: {}", firma_.wartosc_firmy());
            stan_ = 0;
        }
        else
        {
            std::println("Blad! Nieznana komenda. Podaj ponownie komende");
            goto wczytanie_komendy;
        }
    }
    bool get_stan() const { return stan_; }

private:
    bool   stan_;
    Firma& firma_;
};

int main()
{
    double poczatkowy_stan_konta = 100000.;
    Firma  firma{poczatkowy_stan_konta};
    Gra    gra{firma, 1};
    std::println("Stan konta:{}", poczatkowy_stan_konta);
    firma.print_produkcja_info();
    for (int i = 0; i < 10000000; ++i)
    {
        while (1)
        {
            gra.akcja_gracza();
            if (gra.get_stan() == 0)
                break;
        }
        if (firma.wartosc_firmy() > 1000000.)
        {
            std::println("\t\tWygrana!!!!!!!!!!!!!!!!!!");
            break;
        }
        firma.get_stan_konta();
        if (firma.get_stan_konta() <= 0)
        {
            std::println("\t\Przegrana!!!!!!!!!!!!!!!!!!");
            break;
        }
    }
}
