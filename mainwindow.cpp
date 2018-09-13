#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::chargeGraph);
    connect (this, SIGNAL (setStatusBar(QString)),
                ui->statusBar, SLOT (showMessage(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::chargeGraph(){
    QString filename = QFileDialog::getOpenFileName(this,
                                 tr("Open Mesh"),"",
                                 tr("graph file (*.txt)"));

    QString message;
    if(!filename.isEmpty()){
        std::ifstream in(filename.toStdString(), ios::in);
        if (!in){
            emit setStatusBar(message);
            cerr << "Cannot open file" << endl;
            return;
        }

        string line;
        std::getline(in, line);
        if (line[0]=='v'){
            istringstream s(line.substr(8));
            if(!s){
                message="zero vertices or corrupted graph";
                emit setStatusBar(message);
                cerr<<message.toStdString()<<endl;
                return;
            }
            int numberVertices;
            s >> numberVertices;
            if(numberVertices<=0){
                message="zero vertices or corrupted graph";
                emit setStatusBar(message);
                cerr<<message.toStdString()<<endl;
                return;
            }
            cout<<line<<endl;
            adjacency=Eigen::MatrixXi::Constant(numberVertices,numberVertices,-1);
            values.setZero(numberVertices);
            constants.setZero(numberVertices);
            emit setStatusBar("");
        }
        else{
            message="zero vertices or corrupted graph";
            emit setStatusBar(message);
            cerr<<message.toStdString()<<endl;
            return;
        }
        while (std::getline(in, line)){
            istringstream s(line);
            if(s){
                int vi,vj,weight,valuei,valuej,c1,c2;
                s >> vi;
                s >> vj;
                s >> weight;
                s >> valuei;
                s >> valuej;
                s >> c1;
                s >> c2;

                adjacency(vi,vj)=weight;
                adjacency(vj,vi)=weight;
                values(vi)=valuei;
                values(vj)=valuej;
                constants(vi)=c1;
                constants(vj)=c2;
                assert(valuei==0 || valuei==1);
                assert(valuej==0 || valuej==1);
            }
        }
        in.close();
        export_as_dot(adjacency,values,"initial_graph");
        process();
    }
}
void MainWindow::export_as_dot(Eigen::MatrixXi &graph,Eigen::VectorXi &gvalues,QString name){
    int n=graph.rows();
    cout<<"vertices "<<n<<endl;
    string filenamefinal=name.toStdString();
    filenamefinal+=QString::number(n).toStdString()+".txt";
    std::ofstream fileGraph(filenamefinal);
    if (fileGraph.is_open()){
       fileGraph<<"graph { \n";
       for(int j=0;j<graph.rows();j++){
           for(int k=j;k<graph.rows();k++){
               if(graph(j,k)!=-1){
                   if(graph(j,k)==0)
                     fileGraph<<"v"<<j<<" -- "<<"v"<<k<<" [color=green,penwidth=3.0] \n";
                   else if(graph(j,k)==1)
                     fileGraph<<"v"<<j<<" -- "<<"v"<<k<<" [color=red,penwidth=3.0] \n";
               }
           }
       }
       fileGraph<<"{\n";
       for(int j=0;j<gvalues.size();j++){
         if(gvalues(j)==1)
            fileGraph<<"v"<<j<<" [style=filled,fillcolor=red,fontcolor=white]\n";
         if(gvalues(j)==0)
            fileGraph<<"v"<<j<<" [style=filled,fillcolor=green]\n";
       }
       fileGraph<<"}\n";
       fileGraph<<"} \n";
    }
    fileGraph.close();
}
void MainWindow::process(){
   GRBEnv env = GRBEnv();
   GRBModel model = GRBModel(env);

   // Create variables
   int numberEdges=0;
   int numberVertices=values.size();
   for(int j=0;j<adjacency.rows();j++){
       for(int k=0;k<adjacency.rows();k++){
           if(adjacency(j,k)!=-1){
               numberEdges++;
           }
       }
   }
   assert(numberEdges%2==0);
   numberEdges=numberEdges/2; // the adjacency matrix is symmetric
   double elb[numberEdges];
   double clb[numberVertices];
   double erb[numberEdges];
   double crb[numberVertices];
   char binarytypes[numberEdges];
   char intergertypes[numberVertices];
   for(int i=0;i<numberEdges;i++){
       clb[i]=0.0;
       elb[i]=0.0;
   }
   for(int i=0;i<numberEdges;i++){
      binarytypes[i]=GRB_BINARY;
      erb[i]=1.0;
   }
   for(int i=0;i<numberVertices;i++){
      intergertypes[i]=GRB_INTEGER;
      crb[i]=3.0;
   }

   GRBVar* e= model.addVars(elb,erb,NULL,binarytypes,NULL,numberEdges);
   GRBVar* c= model.addVars(clb,crb,NULL,intergertypes,NULL,numberVertices);

   //Enumering the e_i variables
   vector<std::pair<int,int>> ei;
   for(int j=0;j<adjacency.rows();j++){
       for(int k=j;k<adjacency.rows();k++){
           if(adjacency(j,k)!=-1){
              ei.push_back(make_pair(j,k));
           }
       }
   }
   assert(numberEdges==ei.size());
   //Adding constraints
   for(int i=0;i<numberVertices;i++){
       // finding edges from this node
       vector<int> associated;
       for(int j=0;j<ei.size();j++){
           if(ei[j].first==i || ei[j].second==i){
               associated.push_back(j);
           }
       }
       if(!associated.empty()){
           GRBLinExpr lhs=0;
           for(int j = 0; j < associated.size();j++)
               lhs+=e[associated[j]];
           lhs+=-2*c[i];
           model.addConstr(lhs,GRB_EQUAL,double(-constants[i]));  //ej+ek+...ep-2pi=-constant
       }

   }
   // Set objective
   GRBLinExpr funobj=0;
   for(int i=0;i<ei.size();i++){
       funobj+=e[i];
   }
   model.setObjective(funobj,GRB_MAXIMIZE);
   // Optimize model
   model.optimize ();
   for(int i=0;i<ei.size();i++){
       cout<<"e"<<i<<" "<<e[i].get(GRB_DoubleAttr_X)<<endl;
   }

   //Computing the new configuration
   for(int i=0;i<ei.size();i++){
       int newweight=int(e[i].get(GRB_DoubleAttr_X));
       assert(newweight==0 || newweight==1 );
       adjacency(ei[i].first,ei[i].second)=newweight;
       adjacency(ei[i].second,ei[i].first)=newweight;
   }
   for(int i=0;i<numberVertices;i++){
       // finding edges from this node
       vector<int> associated;
       for(int j=0;j<ei.size();j++){
           if(ei[j].first==i || ei[j].second==i){
               associated.push_back(j);
           }
       }
       if(!associated.empty()){
           int cyclesum=constants[i];
           for(int j = 0; j < associated.size();j++){
               int newweight=int(e[associated[j]].get(GRB_DoubleAttr_X));
               cyclesum+=newweight;
           }
           cyclesum=cyclesum%2;
           values(i)=cyclesum;
       }
   }
   export_as_dot(adjacency,values,"optimizedSolution");
   delete [] e;
   delete [] c;
}
