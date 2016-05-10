c HBOOK subroutines
c version 0.00  09-MAY-1997  from  T. Kawabata
c version 1.00  09-MAY-1997  by A. Tamii
C
      subroutine hstini(name)
      COMMON /PAWC/H(32000000)
      character*80 name
      call hlimit(32000000)
      call hropen(1, name, name, 'N', 1024, ISTAT)
      return
      end

      subroutine hstshm(name)
      COMMON /PAWC/H(32000000)
      character*80 name
      call hlimap(1000000,name)
      return
      end

      subroutine hstbk1(id, name, min, max, nbin)
      integer*4    id, nbin
      real*4       min, max
      character*80 name
      call hbook1(id, name, nbin, min, max, 0.)
      return
      end
      
      subroutine hstfl1(id, data)
      integer*4  id
      real*4     data
      call hfill(id, data, 0., 1.)
      return
      end

      subroutine hstbk2(id, name, min1, max1, nbin1, min2, max2, nbin2)
      integer*4    id, nbin1, nbin2
      real*4       min1, max1, min2, max2
      character*80 name
      call hbook2(id, name, nbin1, min1, max1, nbin2, min2, max2, 0.)
      return
      end
      
      subroutine hstfl2(id, x, y)
      integer*4  id
      real*4     x, y
      call hfill(id, x, y, 1.)
      return
      end

      subroutine hstdel(id)
      integer*4  id
      call hdelet(id)
      return
      end

      subroutine hstout(name)
      character*80 name
      call hrput(0, name, 'N')
      return
      end

      subroutine hstend(name)
      character*80 name
      call hrend(name)
      return
      end

