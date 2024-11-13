#include <time.h> 
#include<systemc>
#include <uvm>
using namespace sc_core;
//     UNKNOWN = 0,
//     YUMMY,
//     YUCKY
// };

typedef enum { NO_FLAVOR, APPLE, BLUEBERRY, BUBBLE_GUM, CHOCOLATE } Flavor;
typedef enum { RED, GREEN, BLUE } Color;
typedef enum { UNKNOWN, YUMMY, YUCKY } Taste;

namespace uvm {
// ------------------------
// IF
// ------------------------
class jelly_bean_if : public sc_interface {
public:
    sc_signal<Flavor> flavor;
    sc_signal<Color> color;
    sc_signal<bool> sugar_free;
    sc_signal<bool> sour;
    sc_signal<Taste> taste;
    sc_signal<bool> clk;

    SC_HAS_PROCESS(jelly_bean_if);

    jelly_bean_if() { }

    void generate_clock() {
        while (true) {
            clk.write(true);
            wait(1, sc_core::SC_NS); // High phase
            clk.write(false);
            wait(1, sc_core::SC_NS); // High phase
        }
    }
};
// ------------------------
// Transcation
// ------------------------
    class jelly_bean_transaction : public uvm_sequence_item
    {
        public:
        Flavor  flavor;
        Color   color;
        bool    sugar_free;
        bool    sour;
        Taste   taste;

        public:
        jelly_bean_transaction(const std::string& name_ = "jelly_bean_seq_item" ) : uvm_sequence_item( name_ )    {};

        UVM_OBJECT_UTILS(jelly_bean_transaction);

        //randomise
        void randomize()
        {
            srand (time(NULL));
            flavor  = static_cast<Flavor>   (rand() % 5 );
            color   = static_cast<Color>    (rand() % 3 );
            // taste   = static_cast<Taste>    (rand() % 3 );
            sugar_free =  (rand() % 2 );
            sour =  (rand() % 2 );
        }

        // Print method
        virtual void do_print(const uvm_printer& printer) const override 
        {
            printer.print_field_int("Flavor",static_cast<int>(flavor));
            printer.print_field_int("Color", static_cast<int>(color));
            // printer.print_field_int("Taste", static_cast<int>(taste));
            printer.print_field_int("Sugar", sugar_free);
            printer.print_field_int("Sour", sour);
        }
    };

    class sugar_free_jelly_bean_transaction : public jelly_bean_transaction
    {
        public:
        sugar_free_jelly_bean_transaction (const std::string& name_ = "no_sugar_jelly_bean_seq_item" ) : jelly_bean_transaction(name_)
        { sugar_free = 1; };
    };

// ------------------------
// Sequences
// ------------------------
    template <typename REQ = uvm::jelly_bean_transaction, typename RSP = REQ>
    class one_jelly_bean_sequence : public uvm_sequence<REQ,RSP>
    {     
        private:
        int my_id;
        static int g_my_id;

        public:
        // Constructor
        one_jelly_bean_sequence( const std::string& name_ ) : uvm::uvm_sequence<REQ,RSP>( name_ )
        {
            my_id = g_my_id++;
            printf("Sequence!\n");
        }

        UVM_OBJECT_PARAM_UTILS(one_jelly_bean_sequence<REQ,RSP>);

        void body()
        {
            RSP  * rsp = new RSP();

            jelly_bean_transaction * jb_tx;
            jb_tx           = new REQ();

            this->wait_for_grant(); // this->start_item(jb_tx);
            jb_tx->randomize();
            this->send_request(jb_tx); // this->finish_item(jb_tx);
            UVM_INFO(this->get_name(), "Sent request", uvm::UVM_NONE);
            jb_tx->print();
            this->get_response(rsp);

            delete jb_tx;
        }
    };

    template <typename REQ,typename RSP>
    int one_jelly_bean_sequence<REQ,RSP>::g_my_id = 1;

    template <typename REQ = uvm::jelly_bean_transaction, typename RSP = REQ>
    class same_flavored_jelly_beans_sequence  : public uvm_sequence<REQ,RSP>
    {
        private:
            int num_jelly_beans = 5;
            int my_id;
            static int g_my_id;
        
        public:
        same_flavored_jelly_beans_sequence( const std::string& name_ ) : uvm::uvm_sequence<REQ,RSP>( name_ )
        {
            my_id = g_my_id++;
        };

        UVM_OBJECT_PARAM_UTILS(same_flavored_jelly_beans_sequence<REQ,RSP>);

        void body()
        {
            jelly_bean_transaction * jb_tx;
            jb_tx           = new REQ();
            jb_tx->randomize();
            Flavor flavor = jb_tx->flavor;
            delete jb_tx;
            
            for(int i = 0; i < num_jelly_beans; i++ )
            {
                jelly_bean_transaction * jb_tx;
                jb_tx           = new REQ();

                this->wait_for_grant();
                jb_tx->randomize(); jb_tx->flavor = flavor;
                this->send_request(jb_tx);
                // UVM_INFO(this->get_name(), "Sent request", uvm::UVM_NONE);
                // jb_tx->print();

                // this->get_response(rsp);
                delete jb_tx;
            }
        }
    };

    template <typename REQ, typename RSP>
    int same_flavored_jelly_beans_sequence<REQ,RSP>::g_my_id = 1;


template <typename REQ = uvm::jelly_bean_transaction, typename RSP = REQ>
    class gift_boxed_jelly_beans_sequence  : public uvm_sequence<REQ,RSP>
    {
        private:
            int num_jelly_beans = 10;
            int my_id;
            static int g_my_id;
        
        public:
        gift_boxed_jelly_beans_sequence( const std::string& name_ ) : uvm::uvm_sequence<REQ,RSP>( name_ )
        {
            my_id = g_my_id++;
        };

        UVM_OBJECT_PARAM_UTILS(gift_boxed_jelly_beans_sequence<REQ,RSP>);

        void randomize()
        {
            srand (time(NULL));
            num_jelly_beans = (rand() % 5 );
        }

        void body()
        {      
            same_flavored_jelly_beans_sequence<> * jb_seq;  
            for(int i = 0; i < num_jelly_beans; i++ )
            {
                jb_seq = same_flavored_jelly_beans_sequence<>::type_id::create("sequence");
                jb_seq->print();
                jb_seq->start(this->m_sequencer);

                // UVM_INFO(this->get_name(), "Sent request", uvm::UVM_NONE);
                

                // this->get_response(rsp);
            }
        }
    };

template <typename REQ, typename RSP>
    int gift_boxed_jelly_beans_sequence<REQ,RSP>::g_my_id = 1;
// ------------------------
// Sequencer
// ------------------------

template < typename REQ = uvm::jelly_bean_transaction,  typename RSP = REQ >
class jelly_bean_sequencer : public uvm::uvm_sequencer <REQ,RSP> 
{
    public:
    jelly_bean_sequencer( uvm::uvm_component_name name ) : uvm::uvm_sequencer<REQ,RSP> ( name ) {}

    UVM_COMPONENT_PARAM_UTILS(jelly_bean_sequencer<REQ,RSP>);
};

// ------------------------
// Interface
// ------------------------
// ------------------------
// Monitor
// ------------------------
class jelly_bean_monitor : public uvm::uvm_monitor
{
public:
    uvm::uvm_analysis_port<uvm::jelly_bean_transaction> ap;

    jelly_bean_monitor( uvm::uvm_component_name name ) :  uvm::uvm_monitor ( name ), ap("ap") {}

    UVM_COMPONENT_PARAM_UTILS(jelly_bean_monitor);
};

// ------------------------
// Driver
// ------------------------
template < typename REQ = uvm::jelly_bean_transaction,  typename RSP = REQ >
class jelly_bean_driver : public uvm::uvm_driver <REQ,RSP>
{
public:
    jelly_bean_driver ( uvm::uvm_component_name name ) :  uvm::uvm_driver<REQ,RSP>( name ) {}

    jelly_bean_if* jb_vi;

    UVM_COMPONENT_PARAM_UTILS(jelly_bean_driver<REQ,RSP>);

    void build_phase(uvm_phase& phase)
    {
        if (!uvm_config_db<jelly_bean_if*>::get(nullptr, "", "vif", jb_vi)) {
            UVM_FATAL("DRIVER", "Virtual interface not set");
        }
    }

    void run_phase(uvm_phase& phase)
    {
        REQ req;
        RSP rsp;

        uvm::jelly_bean_monitor * mon;
        mon = dynamic_cast<uvm::jelly_bean_monitor*>(this->get_parent()->get_child("mon"));

        for(;;)
        {
            this->seq_item_port->get_next_item(req); // or alternative this->seq_item_port->peek(req)
            
            // write to dut
            jb_vi->flavor.write(req.flavor);
            jb_vi->color.write(req.color);
            jb_vi->sugar_free.write(req.sugar_free);
            jb_vi->sour.write(req.sour);
            // write to mon
            mon->ap.write(req);
            
            wait(jb_vi->clk.posedge_event());
            this->seq_item_port->item_done(); // or alternative this->seq_item_port->get(tmp) to flush item from fifo


            rsp.set_id_info(req);
            this->seq_item_port->put_response(rsp);  // or alternative: this->seq_item_port->put(rsp)
        }
    }
};


// ------------------------
// Agent
// ------------------------
template < typename REQ = uvm::jelly_bean_transaction,  typename RSP = REQ >
class jelly_bean_agent : public uvm::uvm_agent
{
public:
    uvm::jelly_bean_driver<REQ,RSP> *       drv;
    uvm::jelly_bean_monitor *               mon;
    uvm::jelly_bean_sequencer<REQ,RSP> *    sqr;

    uvm::uvm_analysis_port<uvm::jelly_bean_transaction> ap;

    UVM_COMPONENT_PARAM_UTILS(jelly_bean_agent<REQ,RSP>);

    jelly_bean_agent ( uvm::uvm_component_name name ) :  uvm::uvm_agent( name ), sqr (NULL), drv (NULL), mon(NULL), ap ("ap")
    {}

    virtual void build_phase(uvm::uvm_phase & phase)
    {
        sqr = jelly_bean_sequencer<REQ,RSP>::type_id::create("sqr", this);
        drv = jelly_bean_driver<REQ,RSP>::type_id::create("drv", this);
        mon = jelly_bean_monitor::type_id::create("mon", this);
    }

    virtual void connect_phase(uvm::uvm_phase& phase)
    {
        drv->seq_item_port.connect(sqr->seq_item_export);
        mon->ap.connect(ap);
        // drv->seqr_port.connect(sqr->seq_item_export);
    }

};

// ------------------------
// subscriber & scoreboard
// ------------------------
// Templated subscriber class
template <typename REQ = uvm::jelly_bean_transaction, typename RSP = REQ>
class jelly_bean_sb_subscriber : public uvm_subscriber<REQ> {
public:
    UVM_COMPONENT_UTILS(jelly_bean_sb_subscriber);

    // Constructor
    jelly_bean_sb_subscriber(uvm_component_name name) : uvm_subscriber<REQ>(name) {}

    // Override write method to receive transactions
    virtual void write(const REQ& trans) override;
};

// Templated scoreboard class
template <typename REQ = uvm::jelly_bean_transaction, typename RSP = REQ>
class jelly_bean_scoreboard : public uvm_scoreboard {
public:
    UVM_COMPONENT_UTILS(jelly_bean_scoreboard);

    jelly_bean_sb_subscriber<REQ, RSP>* sub;
    uvm_analysis_export<REQ> ap;

    // Constructor
    jelly_bean_scoreboard(uvm_component_name name)
        : uvm_scoreboard(name), ap("ap") {}

    // Build phase to instantiate the subscriber
    virtual void build_phase(uvm_phase& phase) override;

    // Connect phase to connect the subscriber to the analysis port
    virtual void connect_phase(uvm_phase& phase) override;

    // Method to check the quality of the jelly bean transaction
    virtual void check_jelly_bean_taste(const REQ& jb_tx);
};

// Implementation of subscriber's write method
template <typename REQ, typename RSP>
void jelly_bean_sb_subscriber<REQ, RSP>::write(const REQ& trans) {
    // Retrieve the scoreboard instance
    auto* sb = dynamic_cast<jelly_bean_scoreboard<REQ, RSP>*>(this->get_parent());
    if (sb) {
        UVM_INFO(this->get_name(), "Received transaction in subscriber", UVM_MEDIUM);
        trans.print();
        sb->check_jelly_bean_taste(trans);
    } else {
        UVM_FATAL(this->get_name(), "Failed to cast parent to scoreboard.");
    }
}

// Implementation of scoreboard's build_phase
template <typename REQ, typename RSP>
void jelly_bean_scoreboard<REQ, RSP>::build_phase(uvm_phase& phase) {
    sub = jelly_bean_sb_subscriber<REQ, RSP>::type_id::create("subscriber", this);
    if (!sub) {
        UVM_FATAL(this->get_name(), "Failed to create subscriber.");
    }
}

// Implementation of scoreboard's connect_phase
template <typename REQ, typename RSP>
void jelly_bean_scoreboard<REQ, RSP>::connect_phase(uvm_phase& phase) {
    this->ap.connect(sub->analysis_export);
}

// Implementation of the check_jelly_bean_taste method
template <typename REQ, typename RSP>
void jelly_bean_scoreboard<REQ, RSP>::check_jelly_bean_taste(const REQ& jb_tx) {
    UVM_INFO(this->get_name(), "!! QUALITY CHECK !!", UVM_MEDIUM);
}

// ------------------------
// Environment
// ------------------------
template < typename REQ = uvm::jelly_bean_transaction,  typename RSP = REQ >
class jelly_bean_env : public uvm::uvm_env
{
public:
    UVM_COMPONENT_UTILS(jelly_bean_env);

    jelly_bean_agent<>*    agent;
    jelly_bean_scoreboard<>*    sb;

    jelly_bean_env ( uvm::uvm_component_name name ) :  uvm_env(name) {}

    virtual void build_phase(uvm_phase& phase) override 
    {
        agent = jelly_bean_agent<>::type_id::create("agent", this);
        sb   = jelly_bean_scoreboard<>::type_id::create("scoreboard", this);
    }

    virtual void connect_phase(uvm_phase& phase) override 
    {
        agent->mon->ap.connect(sb->ap);
    }
};

// ------------------------
// Configuration
// ------------------------

class jelly_bean_configuration : public uvm_object
{
public:
    UVM_OBJECT_UTILS(jelly_bean_configuration);

    jelly_bean_configuration (const std::string& name_ = "") : uvm_object ( name_ ) {}

};



// ------------------------
// Test
// ------------------------
    class jelly_bean_test : public uvm_test 
    {
    public:
        UVM_COMPONENT_UTILS(jelly_bean_test);

        jelly_bean_env<>*    env;
        // jelly_bean_taster*   dut;
        jelly_bean_if*       jb_if;
        sc_clock* clk;

        //constructor
        jelly_bean_test(uvm_component_name name) : uvm_test(name) {}

        // Build phase to instantiate the sequencer
        virtual void build_phase(uvm_phase& phase) override 
        {
            uvm_test::build_phase(phase);

            jelly_bean_configuration * jb_cfg = jelly_bean_configuration::type_id::create("jb_cfg");
             // Set configuration via uvm_config_db
            uvm_config_db<jelly_bean_configuration*>::set(this, "*", "config", jb_cfg);

            //transcation override
            jelly_bean_transaction::type_id::set_type_override(          sugar_free_jelly_bean_transaction::get_type());

            //create environment
            env = jelly_bean_env<>::type_id::create("Env", this);
        }

        virtual void run_phase(uvm_phase& phase) override {
            // Start the phase
            phase.raise_objection(this);

            // Create and start the sequence
            gift_boxed_jelly_beans_sequence<> * sequence;
            sequence = gift_boxed_jelly_beans_sequence<>::type_id::create("sequence");
            sequence->randomize();
            sequence->start(env->agent->sqr);
            UVM_INFO(this->get_name(), "jelly_bean_test", UVM_LOW);

            phase.drop_objection(this);
        }
    };
}

// ------------------------
// DUT
// ------------------------
SC_MODULE(jelly_bean_taster)
{
    public:
    sc_in<bool> clk;
    sc_in<Color> color;
    sc_in<Flavor> flavor;
    sc_in<bool> sour;
    sc_in<bool> sugar_free;

    sc_out<Taste> taste;

    SC_CTOR(jelly_bean_taster) {
        SC_METHOD(tasting);
        sensitive << clk.pos(); // Trigger on positive edge of the clock
        dont_initialize();
    }

    // private:
    void tasting()
    {
        if ((flavor.read() == Flavor::CHOCOLATE) && sour.read())
            taste.write(Taste::YUCKY);
        else
            taste.write(Taste::YUMMY);
        
        std::cout << "Taste updated at: " << sc_time_stamp() << std::endl;

    }
};

// ------------------------
// Top/Main
// ------------------------
int sc_main(int, char*[])
{
// Set time resolution
    sc_core::sc_set_time_resolution(1, sc_core::SC_FS);

    // Create the virtual interface
    uvm::jelly_bean_if * vif = new uvm::jelly_bean_if();

    // Instantiate the DUT
    jelly_bean_taster * dut = new jelly_bean_taster("dut");
    dut->clk(vif->clk);
    dut->flavor(vif->flavor);
    dut->color(vif->color);
    dut->sugar_free(vif->sugar_free);
    dut->sour(vif->sour);
    dut->taste(vif->taste);

    uvm::uvm_config_db<uvm::jelly_bean_if*>::set(nullptr, "", "vif", vif);

    sc_spawn_options spawn_opts;
    sc_spawn([vif]() { vif->generate_clock(); }, "clock_generator", &spawn_opts);

    uvm::jelly_bean_test* test = uvm::jelly_bean_test::type_id::create("test");
    uvm::run_test();

    return 0;
}