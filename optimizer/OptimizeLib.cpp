
#include "OptimizeLib.h"

void evaluation_mnbrak(optimizedFunction* func,double* ax,double* bx,double* cx,double* fa,double* fb,double* fc){
	double ulim,u,r,q,fu,dum;
	func->evaluateFunction(ax,*fa);
	func->evaluateFunction(bx,*fb);
	if(*fa<*fb){
		double temp;
		temp=*fa;
		*fa=*fb;
		*fb=temp;
		temp=(*ax);
		(*ax)=(*bx);
		(*bx)=temp;
	}
	(*cx)=(*bx)+GOLD*(*bx-*ax);
	func->evaluateFunction(cx,*fc);
	while(*fb>*fc){
		r=(*bx-*ax)*(*fb-*fc);
		q=(*bx-*cx)*(*fb-*fa);
		u=(*bx)-((*bx-*cx)*q-(*bx-*ax)*r)/(2.0*SIGN(__max(fabs(q-r),TINY),q-r));
		ulim=(*bx)+GLIMIT*(*cx-*bx);

		if((*bx-u)*(u-*cx)>0.0){
			func->evaluateFunction(&u,fu);
			if(fu < *fc){
				*ax=(*bx);
				(*bx)=u;
				*fa=*fb;
				*fb=fu;
				return;
			}else if(fu < *fb){
				*cx=u;
				*fc=fu;
				return;
			}
			u=(*cx)+GOLD*(*cx-*bx);
			func->evaluateFunction(&u,fu);
		}else if((*cx-u)*(u-ulim)>0.0){
			func->evaluateFunction(&u,fu);
			if(fu<*fc){
				SHFT(*bx,*cx,u,*cx+GOLD*(*cx-*bx));
				double fnu;
				func->evaluateFunction(&u,fnu);
				SHFT(*fb,*fc,fu,fnu);
			}
		}else if((u-ulim)*(ulim-*cx)>=0.0){
			u=ulim;
			func->evaluateFunction(&u,fu);
		}else{
			u=(*cx)+GOLD*(*cx-*bx);
			func->evaluateFunction(&u,fu);
		}
		SHFT(*ax,*bx,*cx,u);
		SHFT(*fa,*fb,*fc,fu);
	}
}

double brent_method(optimizedFunction* func,double ax,double bx,double cx,float tol,double& xmin){
	int iter;
	double a,b,d,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm;
	double e=0.0;

	a=(ax<cx?ax:cx);
	b=(ax>cx?ax:cx);
	x=w=v=bx;
	func->evaluateFunction(&x,fx);
	fw=fv=fx;
	for(iter=1;iter<=ITMAX;iter++){
		xm=0.5*(a+b);
		tol2=2.0*(tol1=tol*fabs(x)+ZEPS);
		if(fabs(x-xm)<=(tol2-0.5*(b-a))){
			xmin=x;
			return fx;
		}
		if(fabs(e)>tol1){
			r=(x-w)*(fx-fv);
			q=(x-v)*(fx-fw);
			p=(x-v)*q-(x-w)*r;
			q=2.0*(q-r);
			if(q>0.0)p=-p;
			q=fabs(q);
			etemp=e;
			e=d;
			if(fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x)){
				d=CGOLD*(e=(x>=xm?a-x:b-x));
			}else{
				d=p/q;
				u=x+d;
				if(u-a<tol2||b-u<tol2)
					d=SIGN(tol1,xm-x);
			}
		}else{
			d=CGOLD*(e=(x>=xm?a-x:b-x));
		}
		u=(fabs(d)>=tol1?x+d:x+SIGN(tol1,d));
		func->evaluateFunction(&u,fu);
		if(fu<=fx){
			if(u>=x)a=u;else b=u;
			SHFT(v,w,x,u);
			SHFT(fv,fw,fx,fu);
		}else{
			if(u<x)a=u;else b=u;
			if(fu<=fw||w==x){
				v=w;
				w=u;
				fv=fw;
				fw=fu;
			}else if(fu<=fv||v==x||v==w){
				v=u;
				fv=fu;
			}
		}
	}
	xmin=x;
	return fx;
};


void evaluation_mnbrak(optimizedVectorFunction* func,VectorXd& axv,VectorXd& bxv,VectorXd& cxv,double* fa,double* fb,double* fc){
	double ulim,u,r,q,fu,dum;
	VectorXd stdVct=bxv-axv;
	stdVct.normalize();
	VectorXd origin=bxv;//originを一次元線上の仮の原点としstdVct orientationを正の方向とする
	VectorXd input;
	double ax[1];
	double bx[1];
	double cx[1];
	*bx=0;
	*ax=sqrt((origin-axv).dot(origin-axv));
	*ax=SIGN(*ax,-(origin-axv).dot(stdVct));
	axv=origin+(*ax)*stdVct;
	bxv=origin+(*bx)*stdVct;
	func->evaluateFunction(axv,*fa);
	func->evaluateFunction(bxv,*fb);
	if(*fa<*fb){
		double temp;
		temp=*fa;
		*fa=*fb;
		*fb=temp;
		temp=(*ax);
		(*ax)=(*bx);
		(*bx)=temp;
	}
	(*cx)=(*bx)+GOLD*(*bx-*ax);
	cxv=origin+(*cx)*stdVct;
	func->evaluateFunction(cxv,*fc);
	while(*fb>*fc){
		r=(*bx-*ax)*(*fb-*fc);
		q=(*bx-*cx)*(*fb-*fa);
		u=(*bx)-((*bx-*cx)*q-(*bx-*ax)*r)/(2.0*SIGN(__max(fabs(q-r),TINY),q-r));
		ulim=(*bx)+GLIMIT*(*cx-*bx);

		if((*bx-u)*(u-*cx)>0.0){
			input=origin+u*stdVct;
			func->evaluateFunction(input,fu);
			if(fu < *fc){
				*ax=(*bx);
				(*bx)=u;
				*fa=*fb;
				*fb=fu;
				axv=origin+(*ax)*stdVct;
				bxv=origin+(*bx)*stdVct;
				cxv=origin+(*cx)*stdVct;
				return;
			}else if(fu < *fb){
				*cx=u;
				*fc=fu;
				axv=origin+(*ax)*stdVct;
				bxv=origin+(*bx)*stdVct;
				cxv=origin+(*cx)*stdVct;
				return;
			}
			u=(*cx)+GOLD*(*cx-*bx);
			input=origin+u*stdVct;
			func->evaluateFunction(input,fu);
		}else if((*cx-u)*(u-ulim)>0.0){
			input=origin+u*stdVct;
			func->evaluateFunction(input,fu);
			if(fu<*fc){
				SHFT(*bx,*cx,u,*cx+GOLD*(*cx-*bx));
				double fnu;
				input=origin+u*stdVct;
				func->evaluateFunction(input,fnu);
				SHFT(*fb,*fc,fu,fnu);
			}
		}else if((u-ulim)*(ulim-*cx)>=0.0){
			u=ulim;
			input=origin+u*stdVct;
			func->evaluateFunction(input,fu);
		}else{
			u=(*cx)+GOLD*(*cx-*bx);
			input=origin+u*stdVct;
			func->evaluateFunction(input,fu);
		}
		SHFT(*ax,*bx,*cx,u);
		SHFT(*fa,*fb,*fc,fu);
	}
	axv=origin+(*ax)*stdVct;
				bxv=origin+(*bx)*stdVct;
				cxv=origin+(*cx)*stdVct;
}


double brent_method(optimizedVectorFunction* func,VectorXd& ax,VectorXd& bx,VectorXd& cx,float tol,VectorXd& xmin){
	int iter;
	double a,b,v,w,x,xm,p,q,r,d,etemp,fu,fv,fw,fx,tol1,tol2,u;
	double e=0.0;
	VectorXd stdVct=bx-ax;
	VectorXd origin=bx;//originを一次元線上の仮の原点としstdVct orientationを正の方向とする
	VectorXd input;
	stdVct.normalize();
	double axc=sqrt((origin-ax).dot(origin-ax));
	axc=SIGN(axc,-(origin-ax).dot(stdVct));
	double cxc=sqrt((origin-cx).dot(origin-cx));
	cxc=SIGN(cxc,-(origin-cx).dot(stdVct));
	a=(axc<cxc?axc:cxc);
	b=(axc>cxc?axc:cxc);
	x=w=v=0;
	input=x*stdVct+origin;
	func->evaluateFunction(input,fx);
	fw=fv=fx;
	for(iter=1;iter<=ITMAX;iter++){
		xm=0.5*(a+b);
		tol2=2.0*(tol1=tol*fabs(x)+ZEPS);
		if(fabs(x-xm)<=(tol2-0.5*(b-a))){
			xmin=x*stdVct+origin;
			return fx;
		}
		if(fabs(e)>tol1){
			r=(x-w)*(fx-fv);
			q=(x-v)*(fx-fw);
			p=(x-v)*q-(x-w)*r;
			q=2.0*(q-r);
			if(q>0.0)p=-p;
			q=fabs(q);
			etemp=e;
			e=d;
			if(fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x)){
				d=CGOLD*(e=(x>=xm?a-x:b-x));
			}else{
				d=p/q;
				u=x+d;
				if(u-a<tol2||b-u<tol2)
					d=SIGN(tol1,xm-x);
			}
		}else{
			d=CGOLD*(e=(x>=xm?a-x:b-x));
		}
		u=(fabs(d)>=tol1?x+d:x+SIGN(tol1,d));
		input=u*stdVct+origin;
		func->evaluateFunction(input,fu);
		if(fu<=fx){
			if(u>=x)a=u;else b=u;
			SHFT(v,w,x,u);
			SHFT(fv,fw,fx,fu);
		}else{
			if(u<x)a=u;else b=u;
			if(fu<=fw||w==x){
				v=w;
				w=u;
				fv=fw;
				fw=fu;
			}else if(fu<=fv||v==x||v==w){
				v=u;
				fv=fu;
			}
		}
	}
	xmin=x*stdVct+origin;
	return fx;
};

void powellOptimization(optimizedVectorFunction* func,VectorXd& p,int dim,double& ans,double dtol){
	MatrixXd xi=MatrixXd::Identity(dim,dim);
	int ibig;
	double del,fptt,fp,t;
	VectorXd xit;
	func->evaluateFunction(p,ans);
	cout<<"init,"<<ans<<endl;
	VectorXd pt=p,ptt;//初期点を保存する
//	cout<<iter<<","<<ans<<","<<p(0)<<","<<p(1)<<","<<p(2)<<","<<p(3)<<","<<p(4)<<","<<p(5)<<endl;
	for(int iter=0;;iter++){
		fp=ans;
		ibig=0;
		del=0.0;
		for(int i=0;i<dim;i++){
			xit=xi.block(0,i,dim,1);
			fptt=ans;
			VectorXd ax=p;
			VectorXd bx=p+xit*0.1;
			VectorXd cx(dim);
			double fa[1],fb[1],fc[1];
			evaluation_mnbrak(func,ax,bx,cx,fa,fb,fc);
			ans=brent_method(func,ax,bx,cx,1e-10,p);
			if(fabs(fptt-ans)>del){
				del=fabs(fptt-ans);
				ibig=i;
			}
		}
		if(2.0*fabs(fp-ans)<=dtol*(fabs(fp)+fabs(ans))){//終了判定
			return;
		}
		if(iter==ITMAX)return;
		
			ptt=2.0*p-pt;
			xit=p-pt;
			pt=p;
		
		func->evaluateFunction(ptt,fptt);
		if(fptt<fp){
			t=2.0*(fp-2.0*ans+fptt)*(fp-ans-del)*(fp-ans-del)-del*(fp-fptt)*(fp-fptt);
			if(t<0.0){
				VectorXd ax=p;
				VectorXd bx=p+xit*0.1;
				VectorXd cx(dim);
				double fa[1],fb[1],fc[1];
				cout<<ax<<endl;
				cout<<bx<<endl;
				evaluation_mnbrak(func,ax,bx,cx,fa,fb,fc);
				ans=brent_method(func,ax,bx,cx,1e-10,p);
				for(int j=0;j<dim;j++){
					xi(j,ibig)=xi(j,dim-1);
					xi(j,dim-1)=xit(j);
				}
			}
		}
		//ofstream ofst("E:\\data\\resultLog.txt",ios::app);
		//ofst<<ans<<","<<p(0)<<","<<p(1)<<","<<p(2)<<","<<p(3)<<","<<p(4)<<","<<p(5)<<endl;
		cout << "iteration:" << iter << ", answer: " << ans << endl;
		cout << "parameters:" << endl << p << endl;
		//ofst.close();

	}
}

void steepestDescentOptimization(optimizedVectorDescentFunction* func,VectorXd& p,int dim,double& ans,double alpha,int maxIter){
	VectorXd diff(dim);
	
	for(int iter=0;iter<maxIter;iter++){
		func->evaluateFunction(p,diff,ans);	
		p+=(alpha/(iter+1))*diff;
	}

}

void automaticSteepestDescentOptimization(autoOptimizedVectorDescentFunction* func,VectorXd& p,VectorXd& intervals,int dim,double& ans,double alpha,int maxIter){
	VectorXd diff(dim);
	
	for(int iter=0;iter<maxIter;iter++){
		func->autoEvaluateFunctionAndDiffer(p,diff,intervals,dim,ans);	
		p+=(alpha/(iter+1))*diff;

	}

}

//void automaticSteepestDescentOptimization(autoOptimizedVectorDescentFunction* func,VectorXd& p,VectorXd& intervals,int dim,double& ans,VectorXd& alphas,int maxIter){
//	VectorXd diff(dim);
//	double t=1.0;
//	double div;
//	func->evaluateFunction(p,div);
//	for(int iter=0;iter<maxIter;iter++){
//		t=(maxIter-iter)/(double)maxIter;
//		func->autoEvaluateFunctionAndDiffer(p,diff,intervals,dim,ans);	
//		while(t>(maxIter-iter)/(double)maxIter*0.01){
//			VectorXd p_=p+(alphas*t).cwiseProduct(diff);
//			func->evaluateFunction(p_,div);
//			if(div<ans){
//				p=p_;
//				break;
//			};
//			t*=0.8;
//		}
//		cout<<iter<<": value:"<<div<<endl;
//		if(t<=(maxIter-iter)/(double)maxIter*0.01){
//			break;
//		}
//	}
//
//}

void steepestDescentOptimization(optimizedVectorDescentFunction* func, VectorXd& p, int dim, double& ans, VectorXd alphas, int maxIter) {
	VectorXd diff(dim), diff_(dim);
	double t = 1.0;
	double div;
	func->evaluateFunction(p, diff_, div);
	int terminateCnt = 0;
	for (int iter = 0; iter < maxIter; iter++) {
		func->evaluateFunction(p, diff, ans);
		t = 1.0;// (maxIter - iter) / (double)maxIter;
		cout << iter << ": direction:" << diff.transpose() << endl;
		while (t > 1e-4) {
			VectorXd p_ = p - (alphas*t).cwiseProduct(diff);
			//	cout << p_.transpose() << endl;
			func->evaluateFunction(p_, diff_, div);
			if (div < ans) {
				p = p_;
				double trate = 1.2 * ((1.0 + t) / 2);//adaptive alpha adjustment
				alphas = trate * alphas;
				if (div / ans > 1 - 1e-6) {
					terminateCnt++;
				}
				else terminateCnt = 0;
				break;
			};
			t *= 0.7;
		}

		cout << iter << ": value:" << div << endl;
		cout << p.transpose() << endl;
		if (t <= 1e-4) {
			break;
			//double trate = 1.2 * ((1.0 + t) / 2);//adaptive alpha adjustment
			//terminateCnt++;
		}
		if (terminateCnt >= 3) {
			break;
		}
	}
}



void automaticSteepestDescentOptimization(autoOptimizedVectorDescentFunction* func, VectorXd& p, VectorXd& intervals, int dim, double& ans, VectorXd& alphas, int maxIter) {
	VectorXd diff(dim), diff_(dim);
	double t = 1.0;
	double div;
//	func->evaluateFunction(p, diff_, div);
	int terminateCnt = 0;
	for (int iter = 0; iter < maxIter; iter++) {
//		func->evaluateFunction(p, diff, ans);
		func->autoEvaluateFunctionAndDiffer(p, diff, intervals, dim, ans);
		t = 1.0;// (maxIter - iter) / (double)maxIter;
		cout << iter << ": direction:" << diff.transpose() << endl;
		while (t > 1e-4) {
			VectorXd p_ = p - (alphas*t).cwiseProduct(diff);
			//	cout << p_.transpose() << endl;
			func->evaluateFunction(p_, div);
//			func->autoEvaluateFunctionAndDiffer(p, diff_, intervals, dim, div);
			if (div < ans) {
				p = p_;
				double trate = 1.2 * ((1.0 + t) / 2);//adaptive alpha adjustment
				alphas = trate * alphas;
				if (div / ans > 1 - 1e-6) {
					terminateCnt++;
				}
				else terminateCnt = 0;
				break;
			};
			t *= 0.7;
		}

		cout << iter << ": value:" << div << endl;
		cout << p.transpose() << endl;
		if (t <= 1e-4) {
			break;
			//double trate = 1.2 * ((1.0 + t) / 2);//adaptive alpha adjustment
			//terminateCnt++;
		}
		if (terminateCnt >= 3) {
			break;
		}
	}
}