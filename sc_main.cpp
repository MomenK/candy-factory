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
        jelly_bean_transaction(const std::string& name_ = "jelly_bean_seq_item" ) : uvm_sequence_item( name_ )
        { 
            // printf("Jelly Bean\n"); 
        };

        UVM_OBJECT_UTILS(jelly_bean_transaction);


            // Print method
        virtual void do_print(const uvm_printer& printer) const override {
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
        { 
            sugar_free = 1;
            // printf("Sugar free\n"); 
        };
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
            jb_tx->color    = Color::BLUE;
            jb_tx->flavor   = Flavor::APPLE;
            jb_tx->taste    = Taste::YUCKY;

            UVM_INFO(this->get_name(), "Waiting for grant", uvm::UVM_NONE);
            this->wait_for_grant(); // this->start_item(jb_tx);
            
            this->send_request(jb_tx); // this->finish_item(jb_tx);
            UVM_INFO(this->get_name(), "Sent request", uvm::UVM_NONE);
            jb_tx->print();

            this->get_response(rsp);

            std::ostringstream str;
            str << " my_id %d" << my_id;

            UVM_INFO(this->get_name(), str.str(), uvm::UVM_NONE);
            printf(" my_id %d", my_id);

            delete jb_tx;

        }
    };

    template <typename REQ,typename RSP>
    int one_jelly_bean_sequence<REQ,RSP>::g_my_id = 1;

    template <typename REQ = uvm::jelly_bean_transaction, typename RSP = REQ>
    class same_flavored_jelly_beans_sequence  : public uvm_sequence<REQ,RSP>
    {
        private:
            int num_jelly_beans;
            int my_id;
            static int g_my_id;
        
        public:
        same_flavored_jelly_beans_sequence( const std::string& name_ ) : uvm::uvm_sequence<REQ,RSP>( name_ )
        {
            my_id = g_my_id++;
            printf("Same flavored sequence!\n");
        };

        UVM_OBJECT_PARAM_UTILS(same_flavored_jelly_beans_sequence<REQ,RSP>);

        void body()
        {
            
            for(int i = 0; i < num_jelly_beans; i++ )
            {
                jelly_bean_transaction * jb_tx;
                jb_tx           = new REQ();
                jb_tx->color    = Color::BLUE;
                jb_tx->flavor   = Flavor::APPLE;
                jb_tx->taste    = Taste::YUCKY;

                this->wait_for_grant();
                this->send_request(jb_tx);
                // this->get_response(rsp);

                std::ostringstream str;
                str << " my_id %d" << my_id;

                UVM_INFO(this->get_name(), str.str(), uvm::UVM_NONE);
                printf(" my_id %d", my_id);

                delete jb_tx;
            }
        }
    };

    template <typename REQ, typename RSP>
    int same_flavored_jelly_beans_sequence<REQ,RSP>::g_my_id = 1;


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
        one_jelly_bean_sequence<> * sequence;

        // Create and start the sequence
        sequence = one_jelly_bean_sequence<>::type_id::create("sequence");
        sequence->start(sqr);
    }
};

// ------------------------
// subscriber
// ------------------------
template < typename REQ = uvm::jelly_bean_transaction,  typename RSP = REQ >
class jelly_bean_subscriber : public uvm_subscriber<REQ> {
public:
    // Constructor
    UVM_COMPONENT_UTILS(jelly_bean_subscriber);

    jelly_bean_subscriber(uvm_component_name name) : uvm_subscriber<REQ>(name) {}

    // Write method to receive transactions from the analysis port
    virtual void write(const REQ& trans) override {
        // Print received transaction for debugging
        UVM_INFO(this->get_name(), "Received transaction in subscriber", UVM_MEDIUM);
        trans.print();
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
        jelly_bean_agent<>*    agent;
        jelly_bean_subscriber<>*    sub;

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
            agent = jelly_bean_agent<>::type_id::create("agent", this);
            sub   = jelly_bean_subscriber<>::type_id::create("subscribe", this);
        }

        void connect_phase(uvm::uvm_phase& phase)
        {   
            // Move to agent!!
            // driver->seq_item_port(sequencer->seq_item_export);
            agent->mon->ap.connect(sub->analysis_export);
        }

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