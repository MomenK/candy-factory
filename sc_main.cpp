#include <time.h> 
#include<systemc>
#include <uvm>

enum class Flavor : uint8_t {
    NO_FLAVOR = 0,
    APPLE,
    BLUEBERRY,
    BUBBLE_GUM,
    CHOCOLATE
};

// Define color_e with specific values
enum class Color : uint8_t {
    RED = 0,
    GREEN,
    BLUE
};

// Define taste_e with specific values
enum class Taste : uint8_t {
    UNKNOWN = 0,
    YUMMY,
    YUCKY
};

namespace uvm {
// ------------------------
// Transcation
// ------------------------
    class jelly_bean_transaction : public uvm_sequence_item
    {
        public:
        Flavor  flavor;
        Color   color;
        Taste   taste;
        bool    sugar_free;

        public:
        jelly_bean_transaction(const std::string& name_ = "jelly_bean_seq_item" ) : uvm_sequence_item( name_ )    {};

        UVM_OBJECT_UTILS(jelly_bean_transaction);

        //randomise
        void randomize()
        {
            srand (time(NULL));
            flavor  = static_cast<Flavor>   (rand() % 5 );
            color   = static_cast<Color>    (rand() % 3 );
            taste   = static_cast<Taste>    (rand() % 3 );
            sugar_free =  (rand() % 2 );
        }

        // Print method
        virtual void do_print(const uvm_printer& printer) const override 
        {
            printer.print_field_int("Flavor",static_cast<int>(flavor));
            printer.print_field_int("Color", static_cast<int>(color));
            printer.print_field_int("Taste", static_cast<int>(taste));
            printer.print_field_int("Sugar", sugar_free);
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
            int num_jelly_beans = 2;
            int my_id;
            static int g_my_id;
        
        public:
        gift_boxed_jelly_beans_sequence( const std::string& name_ ) : uvm::uvm_sequence<REQ,RSP>( name_ )
        {
            my_id = g_my_id++;
        };

        UVM_OBJECT_PARAM_UTILS(gift_boxed_jelly_beans_sequence<REQ,RSP>);

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

    UVM_COMPONENT_PARAM_UTILS(jelly_bean_driver<REQ,RSP>);

    void run_phase(uvm_phase& phase)
    {
        REQ req;
        RSP rsp;

        uvm::jelly_bean_monitor * mon;
        mon = dynamic_cast<uvm::jelly_bean_monitor*>(this->get_parent()->get_child("mon"));

        for(;;)
        {
            this->seq_item_port->get_next_item(req); // or alternative this->seq_item_port->peek(req)
            rsp.set_id_info(req);
            // write to dut

            // write to mon
            mon->ap.write(req);


            this->seq_item_port->item_done(); // or alternative this->seq_item_port->get(tmp) to flush item from fifo
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

    virtual void run_phase(uvm::uvm_phase& phase)
    {
        gift_boxed_jelly_beans_sequence<> * sequence;

        // Create and start the sequence
        sequence = gift_boxed_jelly_beans_sequence<>::type_id::create("sequence");
        sequence->start(sqr);
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
// Test
// ------------------------

    class my_test : public uvm_test 
    {
    public:
        UVM_COMPONENT_UTILS(my_test);
        // Declare the sequencer pointer
        // jelly_bean_sequencer<> * sequencer;
        // jelly_bean_driver<>*    driver;
        //OR
        jelly_bean_env<>*    env;

        //constructor
        my_test(uvm_component_name name) : uvm_test(name) {}

        // Build phase to instantiate the sequencer
        virtual void build_phase(uvm_phase& phase) override 
        {
            uvm_test::build_phase(phase);

            // Instantiate the sequencer
            // sequencer = jelly_bean_sequencer<>::type_id::create("sequencer", this);
            // driver = jelly_bean_driver<>::type_id::create("slave", this);
            //OR
            env = jelly_bean_env<>::type_id::create("Env", this);
        }

        // void connect_phase(uvm::uvm_phase& phase)
        // {   
        //     // Move to agent!!
        //     // driver->seq_item_port(sequencer->seq_item_export);
        //     agent->mon->ap.connect(sb->ap);
        // }

        virtual void run_phase(uvm_phase& phase) override {
            // Start the phase
            phase.raise_objection(this);

            printf("\nRunning\n");

            // Move to agent!
            // Pointer to the sequence
            // one_jelly_bean_sequence<> * sequence;

            // // Create and start the sequence
            // sequence = one_jelly_bean_sequence<>::type_id::create("sequence");
            // sequence->start(sequencer);

            // Drop the phase objection after the sequence is complete
            phase.drop_objection(this);
        }
    };

}

int sc_main(int, char*[])
{
    sc_core::sc_set_time_resolution( 1, sc_core::SC_FS );

    // uvm::sugar_free_jelly_bean_transaction * my_bean;
    // my_bean = new uvm::sugar_free_jelly_bean_transaction();
    // delete my_bean;

    uvm::run_test("my_test");

    return 0;
}