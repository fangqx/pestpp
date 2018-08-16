node {
    agent any

    stages 
        stage "check out"
            git clone -b develop --depth 1 https://github.com/jtwhite79/pestpp

        stage "build"
            echo "skipped build"

        stage "test
                cd benchmarks
                nosetests -v
                cd ..
}
    

        
    
