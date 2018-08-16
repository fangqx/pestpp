node {
 

    stage 'Checkout'
        git clone -b develop --depth 1 https://github.com/jtwhite79/pestpp
    
    stage 'Build'
        cd benchmarks
        nosetests -v
        cd .. 
}

    

        
    
