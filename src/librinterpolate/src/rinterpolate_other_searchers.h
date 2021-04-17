
/*
 * Alternative search algorithms, not currently in use
 */

#ifdef QUADRATIC_SEARCH
            /*
             * See https://pdfs.semanticscholar.org/3d91/97ecfcc1a16254c8667b0cbd35c93e7f9437.pdf
             * Designed for integers, not accurate at low indices. 
             * Not faster than binary search, so don't use it!
             */
            a=0;
            b=varcount[j];
            while(likely(b-a > 1))
            {
                /*
                 * The following three are equivalent, but the
                 * bit shift is fastest.
                 */
                //c = ( a + b ) / 2;
                //c = a + (b - a) / 2;//use this in case of overflow
                c = (a+b)>>1;
                rinterpolate_counter_t p1 = a+(b-a)/4;
                rinterpolate_counter_t p2 = a+(b-a)*3/4;
                //printf("mid %d p1 %d p2 %d\n",c,p1,p2);
                if(p1==0)
                {
                    a=0;
                    b=1;
                }
                else
                {
                    if(v<*(tj+c*i))
                    {
                        if(v<*(tj+p1*i))
                        {
                            b=p1-1;
                        }
                        else
                        {
                            a=p1+1;
                            b=c-1;
                        }
                    }
                    else
                    {
                        if(v>*(tj+p2*i))
                        {
                            a=p2+1;
                        }
                        else
                        {
                            a=c+1;
                            b=p2-1;
                        }
                    }
                }
                //else b = c; // if(LESS_OR_EQUAL(v,u)) // obviously!
            }
            
            a=Min(a,b);
            b=a+1;
            //printf("QUADRATIC %d %d\n",a,b);
#endif
//            exit(0);
            
#ifdef PULVER_SEARCH
            // find log_2 (N)
            rinterpolate_counter_t N = varcount[j];
            b = N;
            c = 1;
            while (b>>=1)
            {
                c<<=2;
            }
            a=0;
            while(c)
            {
                b = a|c;
                w = Min(b,N);
#ifdef RINTERPOLATE_POINT_ARITHMETIC_J_LOOP
                if(b < N && v >= *(tj+b*i)) a=b;
#else
                if(b < N && v >= table[b*i+j]) a=b;
#endif
                c>>=1; // equivalent to c/=2;
            }
            if(a+1==varcount[j])a--;
            if(a==b)b++;
            //printf("PULVER %d %d\n",a,b);
#endif

            
#ifdef DIRECT_SEARCH
            a=0;
            b=varcount[j];
            while(a<b && v>=*(tj+a*i))
            {
                a++;
            }
            a--;
            if(a==b-1)a--;
            b=a+1;
            //printf("DIRECT %d %d\n",a,b);
#endif
